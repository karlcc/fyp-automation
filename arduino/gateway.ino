//gateway
#include <pins_arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Flash.h>
#include <SD.h>
#include <TinyWebServer.h>
#include <SoftwareSerial.h>


// Define which pins you will use on the Arduino to communicate with RF 
#define RXPIN_RF 3
#define TXPIN_RF 4


// pin 4 is the SPI select pin for the SDcard
const int SD_CS = 4;

// pin 10 is the SPI select pin for the Ethernet
const int ETHER_CS = 10;

// RF Serial MODULE
SoftwareSerial rf(RXPIN_RF, TXPIN_RF);

// Don't forget to modify the IP to an available one on your home network
byte ip[] = { 
  192, 168, 1, 200 };
static uint8_t mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// The initial state of the LED
char command='a';

const char pasw[]="1234"; // change it with your own password


static volatile boolean semaforo=true; // true=GREEN use false=RED


void send_file_name(TinyWebServer& web_server, const char* filename);
boolean file_handler(TinyWebServer& web_server);
boolean buz_handler(TinyWebServer& web_server);
boolean access_handler(TinyWebServer& web_server);
boolean get_buzstatus_handler(TinyWebServer& web_server);
boolean get_status_handler(TinyWebServer& web_server);
boolean index_handler(TinyWebServer& web_server);
boolean get_data(Client& client);
void file_uploader_handler(TinyWebServer& web_server,TinyWebPutHandler::PutAction action, char* buffer, int size);


TinyWebServer::PathHandler handlers[] = {
  {
    "/", TinyWebServer::GET, &index_handler     }
  ,
  {
    "/upload/" "*", TinyWebServer::PUT, &TinyWebPutHandler::put_handler     }
  ,
  {
    "/buz", TinyWebServer::POST, &buz_handler     }
  ,
  {
    "/access", TinyWebServer::POST, &access_handler     }
  ,
  {
    "/buzstatus" "*", TinyWebServer::GET, &get_buzstatus_handler     }
  ,
  {
    "/getstatus" "*", TinyWebServer::GET, &get_status_handler     }
  ,
  /**
   * {
   * "/" "*", TinyWebServer::GET, &file_handler   }
   * ,
   **/
  {
    NULL    }
  ,
};

const char* headers[] = {
  "Content-Length",
  NULL
};

TinyWebServer web = TinyWebServer(handlers, headers);

boolean has_filesystem = true;
Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;

void send_file_name(TinyWebServer& web_server, const char* filename) {
  if (!filename) {
    web_server.send_error_code(404);
    web_server << F("Could not parse URL");
  } 
  else {
    TinyWebServer::MimeType mime_type
      = TinyWebServer::get_mime_type_from_filename(filename);
    web_server.send_error_code(200);
    web_server.send_content_type(mime_type);
    web_server.end_headers();
    if (file.open(&root, filename, O_READ)) {
      Serial << F("Read file "); 
      Serial.println(filename);
      web_server.send_file(file);
      file.close();
    } 
    else {
      web_server << F("Could not find file: ") << filename << "\n";
    }
  }
}

boolean file_handler(TinyWebServer& web_server) {
  char* filename = TinyWebServer::get_file_from_path(web_server.get_path());
  Serial << F("File load:") ; 
  Serial.println(filename);
  send_file_name(web_server, filename);
  free(filename);
  return true;
}

boolean buz_handler(TinyWebServer& web_server) {  
  char ans;
  char ch;
  while(!semaforo){
  };
  semaforo=false;
  web_server.send_error_code(200);
  web_server.send_content_type("text/plain");
  web_server.end_headers();

  Client& client = web_server.get_client();
  while (client.available()) {
    ch = (char)client.read();	    
    // catch only '1' receives 'id=1'
  }
  rf.print(ch);
  delay(500);
  ans=(char)rf.read(); 
  semaforo=true;
  if(ans=='K'){
    Serial << F("... ok ..."); //DEBUG
    return true;
  }
  else{
    Serial << F("!!!!! not K !!!!"); //DEBUG
    return true;
  }
}

boolean access_handler(TinyWebServer& web_server) {  
  char arrived[sizeof(pasw)-1]; // same size in chars of the password 
  web_server.send_error_code(200);
  web_server.send_content_type("text/plain");
  web_server.end_headers();
  Client& client = web_server.get_client();
  while (client.available()) {
    (char)client.read();
    (char)client.read();//p=
    for(int i=0; i<sizeof(pasw)-1;i++){	  
      arrived[i]=(char)client.read();
    }
  }
  arrived[sizeof(pasw)-1]=0;
  if(strcmp(pasw, arrived) == 0) {
    client << F("OK");
    return true;
  }
  else{
    client << F("not allowed !");
    return true;
  } 
}

boolean get_buzstatus_handler(TinyWebServer& web_server) { 
  while(!semaforo){
  };
  semaforo=false;
  web_server.send_error_code(200);
  web_server.send_content_type("text/plain");
  web_server.end_headers();
  Client& client = web_server.get_client();
  command='b';  
  get_data(client);
  semaforo=true;
  return true;
} 


boolean get_status_handler(TinyWebServer& web_server) { 
  while(!semaforo){
  };
  semaforo=false;
  web_server.send_error_code(200);
  web_server.send_content_type("text/plain");
  web_server.end_headers();
  Client& client = web_server.get_client();
  get_data(client);
  semaforo=true;
  return true;  
}


boolean get_data(Client& client){
  rf.flush();
  rf.print(command);
  delay(1000);
  Serial.print("incoming data with command: ");//DEBUG
  Serial.println(command);//DEBUG
  Serial.print(">"); //DEBUG
  while (rf.available()> 0)
  { 
    char inByte = rf.read();
    client << inByte;
    Serial.write(inByte); //DEBUG
  }		 
  Serial.println("<"); //DEBUG
  rf.flush();	 
  command='a';
}

boolean index_handler(TinyWebServer& web_server) {
  send_file_name(web_server, "INDEX.HTM");
  return true;
}

void file_uploader_handler(TinyWebServer& web_server,
TinyWebPutHandler::PutAction action,
char* buffer, int size) {
  static uint32_t start_time;
  static uint32_t total_size;

  switch (action) {
    case TinyWebPutHandler::START:
    start_time = millis();
    total_size = 0;
    if (!file.isOpen()) {
      // File is not opened, create it. First obtain the desired name
      // from the request path.
      char* fname = web_server.get_file_from_path(web_server.get_path());
      if (fname) {
        Serial << F("Creating ") << fname << "\n";
        file.open(&root, fname, O_CREAT | O_WRITE | O_TRUNC);
        free(fname);
      }
    }
    break;

    case TinyWebPutHandler::WRITE:
    if (file.isOpen()) {
      file.write(buffer, size);
      total_size += size;
    }
    break;

    case TinyWebPutHandler::END:
    file.sync();
    Serial << F("Wrote ") << file.fileSize() << F(" bytes in ")
      << millis() - start_time << F(" millis (received ")
        << total_size << F(" bytes)\n");
    file.close();
  }
}

void setup() {
  Serial.begin(9600);
  Serial << F("Free RAM: ") << FreeRam() << "\n";

  //RF BEGIN
  rf.begin(9600);

  pinMode(SS_PIN, OUTPUT);	// set the SS pin as an output
  digitalWrite(SS_PIN, HIGH);	// and ensure SS is high
  pinMode(ETHER_CS, OUTPUT); 	// Set the CS pin as an output
  digitalWrite(ETHER_CS, HIGH); // Turn off the W5100 chip! 
  pinMode(SD_CS, OUTPUT);       // Set the SDcard CS pin as an output
  digitalWrite(SD_CS, HIGH); 	// Turn off the SD card!


  // initialize the SD card.
  // Pass over the speed and Chip select for the SD card
  if (!card.init(SPI_FULL_SPEED, SD_CS)) {
    Serial << F("card failed\n");
    has_filesystem = false;
  }
  // initialize a FAT volume.
  if (!volume.init(&card)) {
    Serial << F("vol.init failed!\n");
    has_filesystem = false;
  }
  if (!root.openRoot(&volume)) {
    Serial << F("openRoot failed");
    has_filesystem = false;
  }

  if (has_filesystem) {
    // Assign our function to `upload_handler_fn'.
    TinyWebPutHandler::put_handler_fn = file_uploader_handler;
  }

  Ethernet.begin(mac, ip);

  // Start the web server.
  web.begin();
  Serial << F("Web server Ready!\n");
}

void loop() {
  //if (has_filesystem) {
    web.process();
  //}
}


