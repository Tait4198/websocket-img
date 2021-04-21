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

        cv::resize(img, img, cv::Size{img.cols / 2, img.rows / 2}, 0, 0, cv::INTER_AREA);

        std::vector<uchar> buf;
        std::vector<int> params;
        params.push_back(cv::IMWRITE_JPEG_QUALITY);
        params.push_back(100);

        cv::imencode(".jpg", img, buf, params);
        // uuid 32
        std::string id = jv["id"].asString();
        buf.insert(buf.end(), id.c_str(), id.c_str() + strlen(id.c_str()));
        // width 6
        std::stringstream ssWidth;
        ssWidth << std::setw(6) << std::setfill('0') << img.cols;
        std::string width = ssWidth.str();
        buf.insert(buf.end(), width.c_str(), width.c_str() + strlen(width.c_str()));
        // height 6
        std::stringstream ssHeight;
        ssHeight << std::setw(6) << std::setfill('0') << img.rows;
        std::string height = ssHeight.str();
        buf.insert(buf.end(), height.c_str(), height.c_str() + strlen(height.c_str()));

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
    this->threadPool = new ThreadPool(4);
}

ImageWebsocket::~ImageWebsocket() {
    delete this->jsonConvert;
    delete this->threadPool;
}
