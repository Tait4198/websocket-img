#include "ImageWebsocket.h"
#include "opencv2/opencv.hpp"
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace std::chrono;

void ImageWebsocket::imageHandle(const WebSocketConnectionPtr &wsConnPtr, Json::Value &jv) {
    if (jv.isMember("path")) {
        auto t1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        std::string path = jv["path"].asString();
        cv::Mat img = cv::imread(path);
        if (img.empty()) {
            printf("Error %s Not Found\n", path.c_str());
            return;
        }
        int width = img.cols;
        int height = img.rows;
        int interpolation = cv::INTER_AREA;
        int quality = 100;
        if (jv.isMember("quality")) {
            quality = jv["quality"].asInt();
        }
        if (jv.isMember("width") && jv.isMember("height")) {
            int nWidth = jv["width"].asInt();
            int nHeight = jv["height"].asInt();
            if (nWidth > width || nHeight > height) {
                interpolation = cv::INTER_CUBIC;
            }
            width = nWidth;
            height = nHeight;
        } else if (jv.isMember("scale")) {
            double scale = jv["scale"].asDouble();
            if (scale > 1) {
                interpolation = cv::INTER_CUBIC;
            }
            width = (int) (width * scale);
            height = (int) (height * scale);
        }

        cv::resize(img, img, cv::Size{width, height}, 0, 0, interpolation);

        std::vector<uchar> buf;
        std::vector<int> params;
        params.push_back(cv::IMWRITE_JPEG_QUALITY);
        params.push_back(quality);

        cv::imencode(".jpg", img, buf, params);
        // uuid 36
        std::string id = jv["id"].asString();
        buf.insert(buf.end(), id.c_str(), id.c_str() + strlen(id.c_str()));
        // width 6
        std::stringstream ssWidth;
        ssWidth << std::setw(6) << std::setfill('0') << img.cols;
        std::string widthStr = ssWidth.str();
        buf.insert(buf.end(), widthStr.c_str(), widthStr.c_str() + strlen(widthStr.c_str()));
        // height 6
        std::stringstream ssHeight;
        ssHeight << std::setw(6) << std::setfill('0') << img.rows;
        std::string heightStr = ssHeight.str();
        buf.insert(buf.end(), heightStr.c_str(), heightStr.c_str() + strlen(heightStr.c_str()));

        buf.insert(buf.end(), buf.begin(), buf.end());

        wsConnPtr->send(reinterpret_cast<char *>(buf.data()), buf.size(), WebSocketMessageType::Binary);

        auto t2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        printf("%s %ld ms\n", path.c_str(), t2 - t1);
    }
}

void ImageWebsocket::handleNewMessage(const WebSocketConnectionPtr &wsConnPtr, std::string &&message,
                                      const WebSocketMessageType &type) {
    if (type == WebSocketMessageType::Text) {
        Json::Value jv;
        if (jsonConvert->convert(message, &jv)) {
            this->threadPool->enqueue(&ImageWebsocket::imageHandle, this, wsConnPtr, jv);
        }
    }
}

void ImageWebsocket::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &wsConnPtr) {
}

void ImageWebsocket::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr) {
}

ImageWebsocket::ImageWebsocket() {
    this->jsonConvert = new JsonConvert;
    this->threadPool = new ThreadPool(8);
}

ImageWebsocket::~ImageWebsocket() {
    delete this->jsonConvert;
    delete this->threadPool;
}
