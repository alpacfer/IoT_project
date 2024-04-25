#include "WebPage.h"

WebPage::WebPage(int* led) {
  /*
  Constructor containing wifi name (ssid), password, write key api and channel id to allow uploading of data
  */
  _led = led;
  
}
void WebPage::handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<html><title>Internet of Things - Demonstration</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>Velkommen til denne WebServer</h1> \ 
      <p>Internet of Things (IoT) er \"tingenes Internet\" - dagligdags ting kommer på nettet og får ny værdi. Det kan løse mange udfordringer.</p> \
      <p>Her kommunikerer du med en webserver på en lille microcontroller af typen Arduino, som i dette tilfælde styrer en digital udgang, som du så igen kan bruge til at styre en lampe, en ventilator, tænde for varmen eller hvad du lyster</p> \
      <p>Klik på nedenstående knap for at tænde eller slukke LED på port D2</p> \
      <form action=\"/LED\" method=\"POST\" ><input type=\"submit\" value=\"Skift tilstand på LED\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      <p>Med en Arduino ESP8266 kan du lave et have a sjove projekter</p> \      
      <p>Vil du vide mere: Kig på hjemmesiden for uddannelsen : <a href=\"www.dtu.dk/net\">Netværksteknologi og it</a></p> \
      </body></html>");
}

void WebPage::handleLED(int *led) {                          // If a POST request is made to URI /LED
  led = !led;                                // Change the state of the LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void WebPage::handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
