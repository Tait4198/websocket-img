#include <drogon/WebSocketController.h>
#include "JsonConvert.h"
#include "ThreadPool.h"

using namespace drogon;

class ImageWebsocket : public drogon::WebSocketController<ImageWebsocket> {
public:
    ImageWebsocket();

    ~ImageWebsocket() override;

    void handleNewMessage(const WebSocketConnectionPtr &,
                          std::string &&,
                          const WebSocketMessageType &) override;

    void handleNewConnection(const HttpRequestPtr &,
                             const WebSocketConnectionPtr &) override;

    void handleConnectionClosed(const WebSocketConnectionPtr &) override;

    WS_PATH_LIST_BEGIN
        //list path definitions here;
        //WS_PATH_ADD("/path","filter1","filter2",...);
        WS_PATH_ADD("/img");
    WS_PATH_LIST_END

    void imageHandle(const WebSocketConnectionPtr &wsConnPtr, Json::Value &jv);

private:
    JsonConvert *jsonConvert;
    ThreadPool *threadPool;
};
