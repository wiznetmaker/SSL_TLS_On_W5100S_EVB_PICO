#ifndef PROGMEM
#define PROGMEM
#endif

const char html_page_start[] PROGMEM = {
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"  // the connection will be closed after completion of the response
    "Refresh: 5\r\n"         // refresh the page automatically every n sec
    "\r\n"
    "<!DOCTYPE HTML>"
    "<html>"
    "<head>"
      "<meta charset='UTF-8'>"
      "<title>W5100s-EVB-PICO SSL/TLS TEST: ChatGPT</title>"  
      "<link rel='icon' href='https://seeklogo.com/images/C/chatgpt-logo-02AFA704B5-seeklogo.com.png' type='image/x-icon'>"
    "</head>"
    "<body>"
    "<p style='text-align:center;'>"
    "<img alt='ChatGPT' src='https://seeklogo.com/images/C/chatgpt-logo-02AFA704B5-seeklogo.com.png' height='200' width='200'>"
    "<h1 align='center'>W5100s-EVB-PICO SSL TEST</h1>" 
    "<h1 align='center'>ChatGPT</h1>" 
    "<div style='width:900px;margin: 0 auto;text-align: center'>"
    "<form action='/' method='post'>"
    "<input type='text' placeholder='Please enter your question' size='35' name='chatgpttext' required='required'/><br><br>"
    "<input type='submit' value='Submit' style='height:30px; width:80px;'/>"
    "</form>"
    "<div style='text-align: left;'>"
    "<h5>"
};

const char html_page_end[] PROGMEM = {
    "</h5>\r\n" 
    "</div>"
    "</div>"
    "</body>\r\n"
    "<html>\r\n"
};
