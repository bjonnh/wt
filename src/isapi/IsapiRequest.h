#include "WebRequest.h"
#include <sstream>
#include <httpext.h>

namespace Wt {
  namespace isapi {

class IsapiServer;

class IsapiRequest : public WebRequest
{
public:
  IsapiRequest(LPEXTENSION_CONTROL_BLOCK ecb, IsapiServer *server,
    bool forceSynchronous);

  ~IsapiRequest();

  // Signal ISAPI that this connection is to be closed and that we're
  // done with it. The IsapiRequest object must be deleted after calling
  // this method.
  void abort();

  // Returns true if the HTTP request was received without errors
  bool isGood();

  virtual bool isSynchronous() const;

  virtual void flush(ResponseState state,
    CallbackFunction callback,
    void *callbackData);

  // Sends a simple text reply
  void sendSimpleReply(int status, const std::string &msg);

  virtual std::istream& in() { return in_; }
  virtual std::ostream& out() { return out_; }
  virtual std::ostream& err() { return err_; }

  virtual void setStatus(int status);

  virtual void setContentType(const std::string& value);

  virtual void addHeader(const std::string& name, const std::string& value);

  virtual void setRedirect(const std::string& url);

  virtual std::string headerValue(const std::string& name) const;

  virtual std::string envValue(const std::string& name) const;

  virtual std::string scriptName() const;

  virtual std::string serverName() const;

  virtual std::string requestMethod() const;

  virtual std::string queryString() const;

  virtual std::string serverPort() const;

  virtual std::string pathInfo() const;

  virtual std::string remoteAddr() const;

  virtual std::string urlScheme() const;

private:
  LPEXTENSION_CONTROL_BLOCK ecb_;
  IsapiServer *server_;
  bool good_;

  bool synchronous_;
  bool reading_;
  DWORD bytesToRead_;
  char buffer_[1024];
  DWORD bufferSize_;

  void processAsyncRead(DWORD cbIO, DWORD dwError, bool first);
  static void WINAPI completionCallback(LPEXTENSION_CONTROL_BLOCK lpECB,
    PVOID pContext, DWORD cbIO, DWORD dwError);
  void writeSync();
  void writeAsync(DWORD cbIO, DWORD dwError, bool first);
  void flushDone();

  std::vector<std::string> writeData_;
  unsigned int writeIndex_; // next index to be written in writeData_
  unsigned int writeOffset_; // offset withing current item of writeData_
  ResponseState flushState_;

  std::stringstream header_, in_, out_, err_;
  bool chunking_;
  bool responseLengthKnown_;
  bool headerSent_;
  void sendHeader();
  enum {HTTP_1_0, HTTP_1_1} version_;
};

}
}

