## ESP google sheets example

The following code (main.cpp) contains a barebones working example for writing to a google sheet using an esp32. 

To get started, there are a few things you'll need to do on the google side of things. Follow this tutorial up until step 3
https://randomnerdtutorials.com/esp32-datalogging-google-sheets/
(this is for another library; that library also works)

You'll need to copy over the priv_key and the google account email provided into secrets.h. 
You'll also need to copy the spreadsheet ID (found in the URL) to the store line in main.cpp
make sure that this sheet is shared with the email address in the google account. 

