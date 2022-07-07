#include "MobileNetV1.hpp"

using namespace cv;
using namespace std;

UltraPerson::UltraPerson(std::string &yolo_path, int input_width, int input_height, int num_thread_ = 4, float score_trheshold_)
{
    width = input_width;
    height = input_height;
    score_threshold = score_threshold_;
    num_thread = num_thread_;

    // Load model
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile("yolo_path");
    // Build the interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model.get(), resolver)(&interpreter);

    interpreter->AllocateTensors();

    interpreter->SetAllowFp16PrecisionForFp32(true);
    interpreter->SetNumThreads(num_thread); // quad core

    // Get the names
    bool result = UltraPerson.getFileContent("COCO_labels.txt");
    if (!result)
    {
        cout << "loading labels failed";
        exit(-1);
    }

}

// UltraPerson::~UltraPerson()
// {
//     ultraface_interpreter->releaseModel();
//     ultraface_interpreter->releaseSession(ultraface_session);
// }

UltraPerson::getFileContent(std::string fileName)
{
    // Open the File
    std::ifstream in(fileName.c_str());
    // Check if object is valid
    if (!in.is_open())
        return false;

    std::string str;
    // Read the next line from File untill it reaches the end.
    while (std::getline(in, str))
    {
        // Line contains string of length > 0 then save it in vector
        if (str.size() > 0)
            Labels.push_back(str);
    }
    // Close The File
    in.close();
    return true;
}

UltraPerson::detect(Mat &src, std::vector<PersonInfo> &personList)
{
    Mat image;
    int cam_width = src.cols;
    int cam_height = src.rows;

    // copy image to input as input tensor
    cv::resize(src, image, Size(300, 300));
    memcpy(interpreter->typed_input_tensor<uchar>(0), image.data, image.total() * image.elemSize());

    //        cout << "tensors size: " << interpreter->tensors_size() << "\n";
    //        cout << "nodes size: " << interpreter->nodes_size() << "\n";
    //        cout << "inputs: " << interpreter->inputs().size() << "\n";
    //        cout << "input(0) name: " << interpreter->GetInputName(0) << "\n";
    //        cout << "outputs: " << interpreter->outputs().size() << "\n";

    interpreter->Invoke(); // run your model

    const float *detection_locations = interpreter->tensor(interpreter->outputs()[0])->data.f;
    const float *detection_classes = interpreter->tensor(interpreter->outputs()[1])->data.f;
    const float *detection_scores = interpreter->tensor(interpreter->outputs()[2])->data.f;
    const int num_detections = *interpreter->tensor(interpreter->outputs()[3])->data.f;

    // there are ALWAYS 10 detections no matter how many objects are detectable
    //cout << "number of detections: " << num_detections << "\n";

    for (int i = 0; i < num_detections; i++)
    {
        int det_index = (int)detection_classes[i] + 1;
        if (det_index == 1) //check that it's a person
        {
            if (detection_scores[i] > score_threshold)
            {

                FaceInfo rects;
                memset(&rects, 0, sizeof(rects));
                rects.y1 = detection_locations[4 * i] * cam_height;
                rects.x1 = detection_locations[4 * i + 1] * cam_width;
                rects.y2 = detection_locations[4 * i + 2] * cam_height;
                rects.x2 = detection_locations[4 * i + 3] * cam_width;

                personList.pushBack(rects);
            }
        }
    }
}

int main(int argc, char **argv)
{
    float f;
    float FPS[16];
    int i, Fcnt = 0;
    Mat frame;
    chrono::steady_clock::time_point Tbegin, Tend;

    for (i = 0; i < 16; i++)
        FPS[i] = 0.0;

    VideoCapture cap(-1);
    if (!cap.isOpened())
    {
        cerr << "ERROR: Unable to open the camera" << endl;
        return 0;
    }

    cout << "Start grabbing, press ESC on Live window to terminate" << endl;
    while (1)
    {
        //        frame=imread("Traffic.jpg");  //need to refresh frame before dnn class detection
        cap >> frame;
        if (frame.empty())
        {
            cerr << "ERROR: Unable to grab from the camera" << endl;
            break;
        }

        Tbegin = chrono::steady_clock::now();

        detect_from_video(frame);

        Tend = chrono::steady_clock::now();
        // calculate frame rate
        f = chrono::duration_cast<chrono::milliseconds>(Tend - Tbegin).count();
        if (f > 0.0)
            FPS[((Fcnt++) & 0x0F)] = 1000.0 / f;
        for (f = 0.0, i = 0; i < 16; i++)
        {
            f += FPS[i];
        }
        putText(frame, format("FPS %0.2f", f / 16), Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255));

        // show output
        //        cout << "FPS" << f/16 << endl;
        imshow("RPi 4 - 1,9 GHz - 2 Mb RAM", frame);

        char esc = waitKey(5);
        if (esc == 27)
            break;
    }

    cout << "Closing the camera" << endl;
    destroyAllWindows();
    cout << "Bye!" << endl;

    return 0;
}