#include<opencv2/opencv.hpp>
#include<algorithm>
#include<iostream>
#include<vector>
#include<map>
#include<getopt.h>

#define SENSORS

#include "ImageProcessor.hpp"
#include "ObjectTracker.hpp"
#include "VideoOutput.hpp"

#ifdef SENSORS
#include "Sensors.hpp"
#endif


int x_counter[2];
int y_counter[2];

int x[4];
int y[4];

int x_crop[2];
int y_crop[2];

int x_interest[2];
int y_interest[2];

enum InteractionAction {
        SET_COUNTERS_AREA,
        SET_PERSPECTIVE_AREA,
        SET_CROP_AREA,
        SET_INTEREST_AREA,
        NONE
};

void ProvidePip(cv::Mat &frame, cv::Mat &fore, cv::Mat &dst) {

	cv::Size pip1Size(std::min(400, frame.size().width), std::min(200, frame.size().height));
	cv::Rect pip1Rect(cv::Point(dst.size().width - pip1Size.width - 20, 20), pip1Size);
	cv::Mat pip1;
	cv::resize(frame, pip1, pip1Size);

	cv::rectangle(dst, pip1Rect, cv::Scalar(0, 0, 255), 5);
	pip1.copyTo(dst(pip1Rect));

	/*cv::Size pip2Size(std::min(300, fore.size().width), std::min(150, fore.size().height));
	cv::Mat rgbFore(fore.size(), CV_8UC3);
	int from_to[]= {0,0, 0,1, 0,2};
	cv::mixChannels(&fore, fore.channels(), &rgbFore, fore.channels(), from_to, 3);
	cv::Rect pip2Rect(cv::Point(20, 122), pip2Size);
	cv::Mat pip2;
	cv::resize(rgbFore, pip2, pip2Size);

	pip2.copyTo(dst(pip2Rect));*/

	//Add bike to image
	/*cv::Mat cyclistImage = cv::imread("cyclist.png", CV_LOAD_IMAGE_UNCHANGED);
	cyclistImage.data[cyclistImage.step* 0 + 0 + 0] = 0xFF;
	cyclistImage.data[cyclistImage.step* 0 + 0 + 1] = 0xFF;
	cyclistImage.data[cyclistImage.step* 0 + 0 + 2] = 0xFF;
	cv::imshow("Mini", cyclistImage);

/*	cv::Mat rgbBg(rgbaBg.rows, rgbaBg.cols, CV_8UC3);
	cv::mixChannels(&rgbaBg, 2, &rgbBg, 1, from_to, 3);
	rgbBg.copyTo(dst(imageRect));*/


//	cyclistImage.copyTo(dst(imageRect));
}

class InteractionHandler {
private:
        static void (*CurrentCallbackFunction)(int x, int y);
public:
        static void Subscribe(std::string windowName) {
                cv::setMouseCallback(windowName.c_str(), InteractionHandler::MouseCallback, NULL);
        }

        static void SetAction(InteractionAction action) {
            switch (action) {
		case SET_COUNTERS_AREA:
                    CurrentCallbackFunction = GetCountersArea;
		break;
                case SET_PERSPECTIVE_AREA:
                    CurrentCallbackFunction = GetPerspectivePoints;
                break;
                case SET_CROP_AREA:
                    CurrentCallbackFunction = GetCropFrame;
                break;
                case SET_INTEREST_AREA:
                    CurrentCallbackFunction = GetInterestArea;
                break;
                case NONE:
                    CurrentCallbackFunction = NULL;
                break;
            }
        }
        
protected:
        static void MouseCallback(int event, int x, int y, int flags, void *data) {
            if (CurrentCallbackFunction) {
                    switch(event) {
                        case(cv::EVENT_LBUTTONDOWN):
                            CurrentCallbackFunction(x, y);
                            break;
                    }
            }
        }      
 
        static void GetCountersArea(int x, int y) {
            static int step = 0;
            static int x_internal[2];
            static int y_internal[2];
            x_internal[step] = x;
            y_internal[step] = y;
            switch (step) {
                case 0:
                    printf("Ponto %d %d\nSelecione Ponto de Contador a Direita\n");
                    break;
                case 1:              
                    step = 0;
                    for (int i = 0; i < sizeof(::x_counter)/sizeof(::x_counter[0]); i++) {
                            ::x_counter[i] = x_internal[i];
                            ::y_counter[i] = y_internal[i];
                    }
                    printf("Selecione Ponto de Perspectiva Inicial\n");
                    InteractionHandler::SetAction(InteractionAction::SET_PERSPECTIVE_AREA);
                    return;                    
            }
            step++;
        }
                
        static void GetPerspectivePoints(int x, int y) {
            static int step = 0;
            static int x_internal[4];
            static int y_internal[4];
            x_internal[step] = x;
            y_internal[step] = y;
            switch (step) {
                case 0:
                    printf("Ponto %d %d\nSelecione 2\n");
                    break;
                case 1:
                    printf("Ponto %d %d\nSelecione 3\n");
                    break;
                case 2:
                    printf("Ponto %d %d\nSelecione 4\n");
                    break;
                case 3:
                    step = 0;
                    for (int i = 0; i < sizeof(::x)/sizeof(::x[0]); i++) {
                            ::x[i] = x_internal[i];
                            ::y[i] = y_internal[i];
                    }
                    printf("Selecione Ponto de Crop Inicial\n");
                    InteractionHandler::SetAction(InteractionAction::SET_CROP_AREA);
                    return;                    
            }
            step++;
        }
        
        static void GetCropFrame(int x, int y) {
            static int step = 0;
            static int x_internal[2];
            static int y_internal[2];
            x_internal[step] = x;
            y_internal[step] = y;
            switch (step) {
                case 0:
                    printf("Ponto %d %d\nSelecione 2\n");
                    break;
                case 1:
                    step = 0;
                    for (int i = 0; i < sizeof(::x_crop)/sizeof(::x_crop[0]); i++) {
                            ::x_crop[i] = x_internal[i];
                            ::y_crop[i] = y_internal[i];
                    }
                    printf("Selecione Ponto de Interesse Inicial\n");
                    InteractionHandler::SetAction(InteractionAction::SET_INTEREST_AREA);
                    return;                    
            }
            step++;
        }
        
         static void GetInterestArea(int x, int y) {
            static int step = 0;
            static int x_internal[2];
            static int y_internal[2];
            x_internal[step] = x;
            y_internal[step] = y;
            switch (step) {
                case 0:
                    printf("Ponto %d %d\nSelecione 2\n");
                    break;
                case 1:
                    step = 0;
                    for (int i = 0; i < sizeof(::x_interest)/sizeof(::x_interest[0]); i++) {
                            ::x_interest[i] = x_internal[i];
                            ::y_interest[i] = y_internal[i];
                    }
                    InteractionHandler::SetAction(InteractionAction::NONE);
                    return;                    
            }
            step++;
        }
};

void (*InteractionHandler::CurrentCallbackFunction)(int x, int y) = NULL;

static void print_usage(std::string program_name) {
	std::cout << program_name << " usage:" << std::endl <<
		"\t--help   (-h): print this message." << std::endl <<
		"\t--source (-s) <file_name>: Specify where data comes from." <<
		std::endl <<
		"\t--record (-r) <file_name>: Record video to filename." <<
		std::endl <<
		"\t--stream (-S) <device>: Streams video via device." <<
		std::endl <<
		"\t--address (-a) <address>: address must be present and must be in " <<
		"in this format: street-number. --address faria_lima-1200." <<
		std::endl;
	return;
}

int main(int argc, char **argv) {

	static struct option long_options[] = {
		{ "source",    required_argument, NULL, 's'},
		{ "record",    required_argument, NULL, 'r'},
		{ "stream",    required_argument, NULL, 'S'},
		{ "address",   required_argument, NULL, 'a'},
		{ "help",      no_argument, NULL, 'h'},
		{ 0,        0,                 0,    0}
	};
	int opt = 0;
	int long_index = 0;
	
	std::string source_file   = ""; //where data comes from
	std::string record_file   = ""; //output file
	std::string stream_device = ""; //device file used to stream data
	std::string address       = ""; //address where program is running on.
	bool help = false;
	bool abort = false;

	while((opt = getopt_long(argc, argv, "s:r:S:h", long_options, &long_index))
			!= -1) {
		switch(opt) {
			case 's':
				source_file = optarg;
				break;
			case 'r':
				record_file = optarg;
				break;
			case 'S':
				stream_device = optarg;
				break;
			case 'a':
				address = optarg;
				break;
			case 'h':
				help = true;
				break;
			default:
				return EXIT_FAILURE;
				break;
		}
	}

	if(help) {
		print_usage((std::string) argv[0]);
		return EXIT_SUCCESS;
	}

	if(address.empty()) {
		std::cout << "You must specify your address. "
			<< "Like --address faria_lima-1280." << std::endl;
		abort = true;

	}

	if(source_file.empty()) {
		std::cout << "You must specify where data comes from." << std::endl <<
			"\tUse --source <source_file>. Or --help to print usage." <<
			std::endl;
		abort = true;
	}

	if( (source_file == record_file)   ||
		(source_file == stream_device) ||
		((record_file == stream_device) && !record_file.empty())
		) {
		std::cout << "Files must not be the same." << std::endl;
		abort = true;
	}

	if(abort)
		return EXIT_FAILURE;

    int fdwr = 0;
	int ret_code = 0;
	const char *c = NULL;
	cv::VideoWriter *output;
	VideoOutput *outputDevice;

    cv::Mat frame;
    cv::Mat full;
    cv::Mat fore;
    cv::VideoCapture cap(source_file);
    
	if(!stream_device.empty()) {
		c = stream_device.c_str();
	    outputDevice = new VideoOutput(c);
	}

    ImageProcessor ip;   
    
    cap >> frame;
    cv::Size frame_size = frame.size();

	if(!record_file.empty())
	    output = new cv::VideoWriter(record_file, CV_FOURCC('M', 'P', 'E', 'G'), 30, frame_size);

    memset(x_counter, 0, sizeof(x_counter));
    memset(y_counter, 0, sizeof(y_counter));
    
    x[0] = 0                ; y[0] = 0;
    x[1] = frame_size.width ; y[1] = 0;
    x[2] = 0                ; y[2] = frame_size.height;
    x[3] = frame_size.width ; y[3] = frame_size.height;
   
    x_crop[0] = 0                 ; y_crop[0] = 0;
    x_crop[1] = frame_size.width  ; y_crop[1] = frame_size.height;
    
    x_interest[0] = 0; y_interest[0] = 0;
    x_interest[1] = 1; y_interest[1] = 1;
    
    //cap >> frame;
    //cv::imwrite( "tmp/fram0.jpg", frame );
    
    int imageNum = 0;
    
    cv::imshow(address, frame);    
    InteractionHandler::Subscribe(address);
    
    printf("Selecione Ponto de Contador a Esquerda\n");
    InteractionHandler::SetAction(InteractionAction::SET_COUNTERS_AREA);

    cv::Rect interestArea(x_interest[0], y_interest[0], x_interest[1] - x_interest[0], y_interest[1] - y_interest[0]);
    ObjectTracker ot(30, 50, interestArea);


#ifdef SENSORS
    SensorData sd;
    std::thread *sensorsThread = StartSensorsThread("/dev/ttyACM0", &sd);
#endif
    while(1) {
		cv::Point lCounter(x_counter[0], y_counter[0]);
		cv::Point rCounter(x_counter[1], y_counter[1]);

		cv::Point2f p0(x[0], y[0]);
		cv::Point2f p1(x[1], y[1]);
		cv::Point2f p2(x[2], y[2]);
		cv::Point2f p3(x[3], y[3]); 
		
		cv::Rect cropFrame(x_crop[0], y_crop[0], x_crop[1] - x_crop[0], y_crop[1] - y_crop[0]); 
		interestArea = cv::Rect(x_interest[0], y_interest[0], x_interest[1] - x_interest[0], y_interest[1] - y_interest[0]);
		ot.SetInterestArea(interestArea);
		
		cap >> frame;
		if(!record_file.empty())
			output->write(frame); //Write avi file
		full = frame.clone();
		ip.PrepareFrame(frame, cropFrame, p0, p1, p2, p3);
		fore = ip.AcquireForeground(frame);
		ip.InsertInterestArea(frame, interestArea);
		//cv::imshow("Fore", fore);
		ot.IterateTracker(frame, fore);
		ot.PrintTotal(full);
		ot.PrintLeftPartial(full, lCounter);
		ot.PrintRightPartial(full, rCounter);
				  
		cv::imshow(address, frame);

#ifdef SENSORS
		char id[20];
		cv::Point ptu(full.size().width-170, full.size().height-50);
		sprintf(id, "Umidade: %s%%", sd.umidity.c_str());
		cv::putText(full, std::string(id),  ptu, CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 2, CV_AA);
		cv::putText(full, std::string(id),  ptu, CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255), 1, CV_AA);

		sprintf(id, "Temperatura: %s C", sd.temperature.c_str());
		cv::Point ptt(full.size().width-170, full.size().height-30);
		cv::putText(full, std::string(id),  ptt, CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 2, CV_AA);
		cv::putText(full, std::string(id),  ptt, CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255), 1, CV_AA);

		sprintf(id, "Temperatura: %s", sd.temperature.c_str());
		cv::Size sz = cv::getTextSize(id, CV_FONT_HERSHEY_PLAIN, 1, 2, NULL);
		cv::Point ptg(full.size().width - 172 + sz.width, full.size().height-30 - (sz.height/2));
		sprintf(id, "o", sd.temperature.c_str());
		cv::putText(full, std::string(id),  ptg, CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 2, CV_AA);
		cv::putText(full, std::string(id),  ptg, CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255), 1, CV_AA);      

#endif
		ProvidePip(frame, fore, full);
		cv::imshow("Full Frame", full);
		if(!stream_device.empty())
			outputDevice->write(frame);

		if(cv::waitKey(30) == 27)
			break;
		
    }
    
    sensorsThread->join();
    return 0;
}
