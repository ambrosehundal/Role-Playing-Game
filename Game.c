nclude <stdio.h>
#include <stdint.h>
#include "Game.h"
#include "Player.h"
#include <string.h>
#include <stdlib.h>


#define INVENTORY_SIZE 4
#define TRUE 1
#define FALSE 0
#define STANDARD_ERROR 0

// The initial room that Game should initialize to.
#define STARTING_ROOM 32

//the base key for decrypting files

#define DECRYPTION_BASE_KEY 122

// These variable describe the maximum string length of the room title and description respectively.
// Note that they don't account for the trailing '\0' character implicit with C-style strings.
#define GAME_MAX_ROOM_TITLE_LENGTH 21
#define GAME_MAX_ROOM_DESC_LENGTH 255


//Game struct
struct GameRoom{
  char title[GAME_MAX_ROOM_TITLE_LENGTH + 1];
  char description[GAME_MAX_ROOM_DESC_LENGTH + 1];
  uint8_t north;
  uint8_t east;
  uint8_t south;
  uint8_t west;
}Room;




//static variables
static uint8_t NewRoom;  
static uint8_t CurrentRoom;  
static FILE *roomfile;
static char buffer[25];
static uint8_t title_length;
static uint8_t total_items_required;
static uint8_t required_items;
static uint8_t count;
static uint8_t counter;
static uint8_t items_in_inventory;
static uint8_t total_items_contained_in_roomfile;
static uint8_t items_contained_in_inventory;
static uint8_t room_version;
static uint8_t description_length;
//static uint8_t Room_Inventory [INVENTORY_SIZE];



/**
 * These function transitions between rooms. Each call should return SUCCESS if the current room has
 * an exit in the correct direction and the new room was able to be loaded, and STANDARD_ERROR
 * otherwise.
 * @return SUCCESS if the room CAN be navigated to and changing the current room to that new room
 *         succeeded.
 */
int GameGoNorth(void){
  if (GameGetCurrentRoomExits() & GAME_ROOM_EXIT_NORTH_EXISTS) {
    // The current room has a north exit.
    fclose(roomfile);
    NewRoom = Room.north; 



    //check if new room was able to be loaded
    //open file
    sprintf(buffer, "RoomFiles/room%d.txt", STARTING_ROOM);
    roomfile = fopen(buffer, "r");

    if(roomfile != NULL) {
      printf("file is present\n");
    }
    if (roomfile == NULL) {
      FATAL_ERROR();
      return 0;
    }




    //read title length first
    fseek(roomfile, 0, SEEK_SET);
    title_length = fgetc(roomfile);
    title_length ^= (STARTING_ROOM + 122); //decrypt it
    printf("title length is %d\n", title_length);

    //read title from file
    fread(Room.title, 1, title_length, roomfile);         
    printf("%s\n", Room.title);


    //Decrypt title for testing
    int i, j;
    char output[title_length + 1];
    for (i = 0; i < title_length ; i++){
      output[i] = Room.title[i] ^ (STARTING_ROOM + 122);               
    } 




    //testing decrypted title letters
    for (j = 0; j < title_length + 1; j++){
      printf("%c",output[j]);
    }
    printf("\n");
    printf("End of encryption\n");




    /********************************************************************************/
    //if Items required are in inventory or not
    /**********************************************************************************/
    //Item Requirements
    // fread(&items_required, 1, 1, roomfile);
    //read item requirement byte from file
    fseek(roomfile, title_length + 1, SEEK_SET); //start at byte after title_length
    //    total_items_required = fgetc(roomfile); //advance to next pointer
    //    total_items_required ^= (STARTING_ROOM + 122); //decrypt it

    //Testing by printing item requirements
    if(total_items_required != 0){
      printf("Total required Items: %d\n", total_items_required);


    }
    //required = fgetc(roomfile); //advance to next pointer
    //required ^= (STARTING_ROOM + 122);
    //printf("Required Items: %x\n", required_items);




    //room_version is initally FALSE, it changes to TRUE once we actually find the room version that has 
    // 0 item requirements or all items required are in player inventory

    room_version = FALSE;


    while(room_version == FALSE){
      //clearing the old DATA
      //      int k;
      //      for(k = 0; k < GAME_MAX_ROOM_DESC_LENGTH; k++){



      // Room.description[GAME_MAX_ROOM_DESC_LENGTH] = '\0';
      memset(Room.description, 0,  GAME_MAX_ROOM_DESC_LENGTH);
      //  memset(Room.description, 0, sizeof (Room.description));
      //  memset(Room.title, 0, sizeof (Room.title));

      //reset all values  
      total_items_required = fgetc(roomfile); //advance to next pointer
      total_items_required ^= (STARTING_ROOM + 122); //decrypt it

      count = 0;
      counter = 0;
      printf("Total items required for this room  %d\n", total_items_required);


      //in case of Room STARTING_ROOM, required item was not found
      /*******************************************************************************/ 
      //if no. of items required  > 0
      //this block of code checks all items required and if they are in player inventory

      if(total_items_required > 0){
        count = total_items_required;
        printf("Total items required for this room  %d\n", total_items_required);
        while(count > 0){
          required_items = fgetc(roomfile);
          required_items ^= (STARTING_ROOM + 122);
          printf("Required Items: %x\n", required_items);
          //check if items required are in inventory
          if(FindInInventory(required_items)== SUCCESS){
            items_in_inventory++;
          }
          printf("No. of items matched in inventory:  %d\n", items_in_inventory);
          count--;
        }
        /***************************************************************************************/
        //once we check all items required and if they are in inventory, move to Description               
        //check description
        description_length = fgetc(roomfile);
        description_length ^= (STARTING_ROOM + 122); //decrypt it
        printf("Description length is %d\n", description_length);
        fread(Room.description, 1, description_length, roomfile);
        char desc[description_length + 1];


        //decrypt the description into desc
        for (i = 0; i < description_length ; i++){
          desc[i] = Room.description[i] ^ (STARTING_ROOM + 122);
        }

        printf("Room Description: ");
        for (j = 0; j < description_length; j++){
          printf("%c",desc[j]);
        }
        /**********************************************/
        printf("\n");
        // static uint8_t items_contained;
        //  static uint8_t items_contained_in_inventory;
        /******************************************************************************/    
        //Items contained
        total_items_contained_in_roomfile = fgetc(roomfile);
        total_items_contained_in_roomfile ^= (STARTING_ROOM + 122);
        counter = total_items_contained_in_roomfile;
        printf("total_items_contained_in_roomfile is %d\n", total_items_contained_in_roomfile);
        printf("Counter is  %d\n", total_items_contained_in_roomfile);    
        while (counter > 0) {
          items_contained_in_inventory = fgetc(roomfile);
          items_contained_in_inventory^= (STARTING_ROOM + 122);
          AddToInventory(items_contained_in_inventory);
          counter--;
        }
        /******************Get Exits********************/
        Room.north = fgetc(roomfile);
        Room.east = fgetc(roomfile);
        Room.south = fgetc(roomfile);
        Room.west = fgetc(roomfile);

      }
      else if(total_items_required == 0){
        //if no. of items required is 0

        //check description
        description_length = fgetc(roomfile);
        description_length ^= (STARTING_ROOM + 122); //decrypt it
        printf("Description length is %d\n", description_length);
        fread(Room.description, 1, description_length, roomfile);
        char desc[description_length + 1];


        //decrypt the description into desc
        for (i = 0; i < description_length ; i++){
          desc[i] = Room.description[i] ^ (STARTING_ROOM + 122);
        }

        printf("Room Description: ");
        for (j = 0; j < description_length; j++){
          printf("%c",desc[j]);
        }
        printf("\n");

        //Items contained
        total_items_contained_in_roomfile = fgetc(roomfile);
        total_items_contained_in_roomfile ^= (STARTING_ROOM + 122);
        printf("Counter is %d\n", total_items_contained_in_roomfile);
        counter = total_items_contained_in_roomfile;
        printf("total_items_contained_in_roomfile is %d\n", total_items_contained_in_roomfile);
        while (counter > 0) {
          items_contained_in_inventory = fgetc(roomfile);
          items_contained_in_inventory^= (STARTING_ROOM + 122);
          printf("total_items_contained_in_inventory is %d\n", items_contained_in_inventory);
          AddToInventory(items_contained_in_inventory);
          counter--;
        }
        /*********Get Exits*******************/
        Room.north = fgetc(roomfile);
        Room.east = fgetc(roomfile);
        Room.south = fgetc(roomfile);
        Room.west = fgetc(roomfile);


        room_version = TRUE;
        return SUCCESS;
      }
      //if player inventory has all items required, the current room version is to be displayed
      if (items_in_inventory == total_items_required) {
        room_version = TRUE;
      } else {
        //Go to next room version
        fseek(roomfile, 0, SEEK_CUR);
      }
    }

    return SUCCESS;

    }

    return STANDARD_ERROR;

  }



  int GameGoEast(void){
    if (GameGetCurrentRoomExits() & GAME_ROOM_EXIT_EAST_EXISTS) {
      // The current room has a north exit.
      fclose(roomfile);
      NewRoom = Room.east; 



      //check if new room was able to be loaded
      //open file
      sprintf(buffer, "RoomFiles/room%d.txt", STARTING_ROOM);
      roomfile = fopen(buffer, "r");

      if(roomfile != NULL) {
        printf("file is present\n");
      }
      if (roomfile == NULL) {
        FATAL_ERROR();
        return 0;
      }




      //read title length first
      fseek(roomfile, 0, SEEK_SET);
      title_length = fgetc(roomfile);
      title_length ^= (STARTING_ROOM + 122); //decrypt it
      printf("title length is %d\n", title_length);

      //read title from file
      fread(Room.title, 1, title_length, roomfile);         
      printf("%s\n", Room.title);


      //Decrypt title for testing
      int i, j;
      char output[title_length + 1];
      for (i = 0; i < title_length ; i++){
        output[i] = Room.title[i] ^ (STARTING_ROOM + 122);               
      } 




      //testing decrypted title letters
      for (j = 0; j < title_length + 1; j++){
        printf("%c",output[j]);
      }
      printf("\n");
      printf("End of encryption\n");




      /********************************************************************************/
      //if Items required are in inventory or not
      /**********************************************************************************/
      //Item Requirements
      // fread(&items_required, 1, 1, roomfile);
      //read item requirement byte from file
      fseek(roomfile, title_length + 1, SEEK_SET); //start at byte after title_length
      //    total_items_required = fgetc(roomfile); //advance to next pointer
      //    total_items_required ^= (STARTING_ROOM + 122); //decrypt it

      //Testing by printing item requirements
      if(total_items_required != 0){
        printf("Total required Items: %d\n", total_items_required);


      }
      //required = fgetc(roomfile); //advance to next pointer
      //required ^= (STARTING_ROOM + 122);
      //printf("Required Items: %x\n", required_items);




      //room_version is initally FALSE, it changes to TRUE once we actually find the room version that has 
      // 0 item requirements or all items required are in player inventory

      room_version = FALSE;


      while(room_version == FALSE){
        //clearing the old DATA
        //      int k;
        //      for(k = 0; k < GAME_MAX_ROOM_DESC_LENGTH; k++){



        // Room.description[GAME_MAX_ROOM_DESC_LENGTH] = '\0';
        memset(Room.description, 0,  GAME_MAX_ROOM_DESC_LENGTH);
        //  memset(Room.description, 0, sizeof (Room.description));
        //  memset(Room.title, 0, sizeof (Room.title));

        //reset all values  
        total_items_required = fgetc(roomfile); //advance to next pointer
        total_items_required ^= (STARTING_ROOM + 122); //decrypt it

        count = 0;
        counter = 0;
        printf("Total items required for this room  %d\n", total_items_required);


        //in case of Room STARTING_ROOM, required item was not found
        /*******************************************************************************/ 
        //if no. of items required  > 0
        //this block of code checks all items required and if they are in player inventory

        if(total_items_required > 0){
          count = total_items_required;
          printf("Total items required for this room  %d\n", total_items_required);
          while(count > 0){
            required_items = fgetc(roomfile);
            required_items ^= (STARTING_ROOM + 122);
            printf("Required Items: %x\n", required_items);
            //check if items required are in inventory
            if(FindInInventory(required_items)== SUCCESS){
              items_in_inventory++;
            }
            printf("No. of items matched in inventory:  %d\n", items_in_inventory);
            count--;
          }
          /***************************************************************************************/
          //once we check all items required and if they are in inventory, move to Description               
          //check description
          description_length = fgetc(roomfile);
          description_length ^= (STARTING_ROOM + 122); //decrypt it
          printf("Description length is %d\n", description_length);
          fread(Room.description, 1, description_length, roomfile);
          char desc[description_length + 1];


          //decrypt the description into desc
          for (i = 0; i < description_length ; i++){
            desc[i] = Room.description[i] ^ (STARTING_ROOM + 122);
          }

          printf("Room Description: ");
          for (j = 0; j < description_length; j++){
            printf("%c",desc[j]);
          }
          /**********************************************/
          printf("\n");
          // static uint8_t items_contained;
          //  static uint8_t items_contained_in_inventory;
          /******************************************************************************/    
          //Items contained
          total_items_contained_in_roomfile = fgetc(roomfile);
          total_items_contained_in_roomfile ^= (STARTING_ROOM + 122);
          counter = total_items_contained_in_roomfile;
          printf("total_items_contained_in_roomfile is %d\n", total_items_contained_in_roomfile);
          printf("Counter is  %d\n", total_items_contained_in_roomfile);    
          while (counter > 0) {
            items_contained_in_inventory = fgetc(roomfile);
            items_contained_in_inventory^= (STARTING_ROOM + 122);
            AddToInventory(items_contained_in_inventory);
            counter--;
          }
          /******************Get Exits********************/
          Room.north = fgetc(roomfile);
          Room.east = fgetc(roomfile);
          Room.south = fgetc(roomfile);
          Room.west = fgetc(roomfile);

        }
        else if(total_items_required == 0){
          //if no. of items required is 0

          //check description
          description_length = fgetc(roomfile);
          description_length ^= (STARTING_ROOM + 122); //decrypt it
          printf("Description length is %d\n", description_length);
          fread(Room.description, 1, description_length, roomfile);
          char desc[description_length + 1];


          //decrypt the description into desc
          for (i = 0; i < description_length ; i++){
            desc[i] = Room.description[i] ^ (STARTING_ROOM + 122);
          }

          printf("Room Description: ");
          for (j = 0; j < description_length; j++){
            printf("%c",desc[j]);
          }
          printf("\n");

          //Items contained
          total_items_contained_in_roomfile = fgetc(roomfile);
          total_items_contained_in_roomfile ^= (STARTING_ROOM + 122);
          printf("Counter is %d\n", total_items_contained_in_roomfile);
          counter = total_items_contained_in_roomfile;
          printf("total_items_contained_in_roomfile is %d\n", total_items_contained_in_roomfile);
          while (counter > 0) {
            items_contained_in_inventory = fgetc(roomfile);
            items_contained_in_inventory^= (STARTING_ROOM + 122);
            printf("total_items_contained_in_inventory is %d\n", items_contained_in_inventory);
            AddToInventory(items_contained_in_inventory);
            counter--;
          }
          /*********Get Exits*******************/
          Room.north = fgetc(roomfile);
          Room.east = fgetc(roomfile);
          Room.south = fgetc(roomfile);
          Room.west = fgetc(roomfile);


          room_version = TRUE;
          return SUCCESS;
        }
        //if player inventory has all items required, the current room version is to be displayed
        if (items_in_inventory == total_items_required) {
          room_version = TRUE;
        } else {
          //Go to next room version
          fseek(roomfile, 0, SEEK_CUR);
        }
      }

      return SUCCESS;

      }

      return STANDARD_ERROR;
    }

    /**
     * @see GameGoNorth
     */
    int GameGoSouth(void){
      if (GameGetCurrentRoomExits() & GAME_ROOM_EXIT_SOUTH_EXISTS) {
        // The current room has a south exit.
        fclose(roomfile);
        NewRoom = Room.south; 



        //check if new room was able to be loaded
        //open file
        sprintf(buffer, "RoomFiles/room%d.txt", STARTING_ROOM);
        roomfile = fopen(buffer, "r");

        if(roomfile != NULL) {
          printf("file is present\n");
        }
        if (roomfile == NULL) {
          FATAL_ERROR();
          return 0;
        }




        //read title length first
        fseek(roomfile, 0, SEEK_SET);
        title_length = fgetc(roomfile);
        title_length ^= (STARTING_ROOM + 122); //decrypt it
        printf("title length is %d\n", title_length);

        //read title from file
        fread(Room.title, 1, title_length, roomfile);         
        printf("%s\n", Room.title);


        //Decrypt title for testing
        int i, j;
        char output[title_length + 1];
        for (i = 0; i < title_length ; i++){
          output[i] = Room.title[i] ^ (STARTING_ROOM + 122);               
        } 




        //testing decrypted title letters
        for (j = 0; j < title_length + 1; j++){
          printf("%c",output[j]);
        }
        printf("\n");
        printf("End of encryption\n");




        /********************************************************************************/
        //if Items required are in inventory or not
        /**********************************************************************************/
        //Item Requirements
        // fread(&items_required, 1, 1, roomfile);
        //read item requirement byte from file
        fseek(roomfile, title_length + 1, SEEK_SET); //start at byte after title_length
        //    total_items_required = fgetc(roomfile); //advance to next pointer
        //    total_items_required ^= (STARTING_ROOM + 122); //decrypt it

        //Testing by printing item requirements
        if(total_items_required != 0){
          printf("Total required Items: %d\n", total_items_required);


        }
        //required = fgetc(roomfile); //advance to next pointer
        //required ^= (STARTING_ROOM + 122);
        //printf("Required Items: %x\n", required_items);




        //room_version is initally FALSE, it changes to TRUE once we actually find the room version that has 
        // 0 item requirements or all items required are in player inventory

        room_version = FALSE;


        while(room_version == FALSE){
          //clearing the old DATA
          //      int k;
          //      for(k = 0; k < GAME_MAX_ROOM_DESC_LENGTH; k++){



          // Room.description[GAME_MAX_ROOM_DESC_LENGTH] = '\0';
          memset(Room.description, 0,  GAME_MAX_ROOM_DESC_LENGTH);
          //  memset(Room.description, 0, sizeof (Room.description));
          //  memset(Room.title, 0, sizeof (Room.title));

          //reset all values  
          total_items_required = fgetc(roomfile); //advance to next pointer
          total_items_required ^= (STARTING_ROOM + 122); //decrypt it

          count = 0;
          counter = 0;
          printf("Total items required for this room  %d\n", total_items_required);


          //in case of Room STARTING_ROOM, required item was not found
          /*******************************************************************************/ 
          //if no. of items required  > 0
          //this block of code checks all items required and if they are in player inventory

          if(total_items_required > 0){
            count = total_items_required;
            printf("Total items required for this room  %d\n", total_items_required);
            while(count > 0){
              required_items = fgetc(roomfile);
              required_items ^= (STARTING_ROOM + 122);
              printf("Required Items: %x\n", required_items);
              //check if items required are in inventory
              if(FindInInventory(required_items)== SUCCESS){
                items_in_inventory++;
              }
              printf("No. of items matched in inventory:  %d\n", items_in_inventory);
              count--;
            }
            /***************************************************************************************/
            //once we check all items required and if they are in inventory, move to Description               
            //check description
            description_length = fgetc(roomfile);
            description_length ^= (STARTING_ROOM + 122); //decrypt it
            printf("Description length is %d\n", description_length);
            fread(Room.description, 1, description_length, roomfile);
            char desc[description_length + 1];


            //decrypt the description into desc
            for (i = 0; i < description_length ; i++){
              desc[i] = Room.description[i] ^ (STARTING_ROOM + 122);
            }

            printf("Room Description: ");
            for (j = 0; j < description_length; j++){
              printf("%c",desc[j]);
            }
            /**********************************************/
            printf("\n");
            // static uint8_t items_contained;
            //  static uint8_t items_contained_in_inventory;
            /******************************************************************************/    
            //Items contained
            total_items_contained_in_roomfile = fgetc(roomfile);
            total_items_contained_in_roomfile ^= (STARTING_ROOM + 122);
            counter = total_items_contained_in_roomfile;
            printf("total_items_contained_in_roomfile is %d\n", total_items_contained_in_roomfile);
            printf("Counter is  %d\n", total_items_contained_in_roomfile);    
            while (counter > 0) {
              items_contained_in_inventory = fgetc(roomfile);
              items_contained_in_inventory^= (STARTING_ROOM + 122);
              AddToInventory(items_contained_in_inventory);
              counter--;
            }
            /******************Get Exits********************/
            Room.north = fgetc(roomfile);
            Room.east = fgetc(roomfile);
            Room.south = fgetc(roomfile);
            Room.west = fgetc(roomfile);

          }
          else if(total_items_required == 0){
            //if no. of items required is 0

            //check description
            description_length = fgetc(roomfile);
            description_length ^= (STARTING_ROOM + 122); //decrypt it
            printf("Description length is %d\n", description_length);
            fread(Room.description, 1, description_length, roomfile);
            char desc[description_length + 1];


            //decrypt the description into desc
            for (i = 0; i < description_length ; i++){
              desc[i] = Room.description[i] ^ (STARTING_ROOM + 122);
            }

            printf("Room Description: ");
            for (j = 0; j < description_length; j++){
              printf("%c",desc[j]);
            }
            printf("\n");

            //Items contained
            total_items_contained_in_roomfile = fgetc(roomfile);
            total_items_contained_in_roomfile ^= (STARTING_ROOM + 122);
            printf("Counter is %d\n", total_items_contained_in_roomfile);
            counter = total_items_contained_in_roomfile;
            printf("total_items_contained_in_roomfile is %d\n", total_items_contained_in_roomfile);
            while (counter > 0) {
              items_contained_in_inventory = fgetc(roomfile);
              items_contained_in_inventory^= (STARTING_ROOM + 122);
              printf("total_items_contained_in_inventory is %d\n", items_contained_in_inventory);
              AddToInventory(items_contained_in_inventory);
              counter--;
            }
            /*********Get Exits*******************/
            Room.north = fgetc(roomfile);
            Room.east = fgetc(roomfile);
            Room.south = fgetc(roomfile);
            Room.west = fgetc(roomfile);


            room_version = TRUE;
            return SUCCESS;
          }
          //if player inventory has all items required, the current room version is to be displayed
          if (items_in_inventory == total_items_required) {
            room_version = TRUE;
          } else {
            //Go to next room version
            fseek(roomfile, 0, SEEK_CUR);
          }
        }

        return SUCCESS;

        }

        return STANDARD_ERROR;
      }

      /**
       * @see GameGoNorth
       */
      int GameGoWest(void){
        if (GameGetCurrentRoomExits() & GAME_ROOM_EXIT_WEST_EXISTS) {
          // The current room has a north exit.
          fclose(roomfile);
          NewRoom = Room.west; 



          //check if new room was able to be loaded
          //open file
          sprintf(buffer, "RoomFiles/room%d.txt", STARTING_ROOM);
          roomfile = fopen(buffer, "r");

          if(roomfile != NULL) {
            printf("file is present\n");
          }
          if (roomfile == NULL) {
            FATAL_ERROR();
            return 0;
          }




          //read title length first
          fseek(roomfile, 0, SEEK_SET);
          title_length = fgetc(roomfile);
          title_length ^= (STARTING_ROOM + 122); //decrypt it
          printf("title length is %d\n", title_length);

          //read title from file
          fread(Room.title, 1, title_length, roomfile);         
          printf("%s\n", Room.title);


          //Decrypt title for testing
          int i, j;
          char output[title_length + 1];
          for (i = 0; i < title_length ; i++){
            output[i] = Room.title[i] ^ (STARTING_ROOM + 122);               
          } 




          //testing decrypted title letters
          for (j = 0; j < title_length + 1; j++){
            printf("%c",output[j]);
          }
          printf("\n");
          printf("End of encryption\n");




          /********************************************************************************/
          //if Items required are in inventory or not
          /**********************************************************************************/
          //Item Requirements
          // fread(&items_required, 1, 1, roomfile);
          //read item requirement byte from file
          fseek(roomfile, title_length + 1, SEEK_SET); //start at byte after title_length
          //    total_items_required = fgetc(roomfile); //advance to next pointer
          //    total_items_required ^= (STARTING_ROOM + 122); //decrypt it

          //Testing by printing item requirements
          if(total_items_required != 0){
            printf("Total required Items: %d\n", total_items_required);


          }
          //required = fgetc(roomfile); //advance to next pointer
          //required ^= (STARTING_ROOM + 122);
          //printf("Required Items: %x\n", required_items);




          //room_version is initally FALSE, it changes to TRUE once we actually find the room version that has 
          // 0 item requirements or all items required are in player inventory

          room_version = FALSE;


          while(room_version == FALSE){
            //clearing the old DATA
            //      int k;
            //      for(k = 0; k < GAME_MAX_ROOM_DESC_LENGTH; k++){



            // Room.description[GAME_MAX_ROOM_DESC_LENGTH] = '\0';
            memset(Room.description, 0,  GAME_MAX_ROOM_DESC_LENGTH);
            //  memset(Room.description, 0, sizeof (Room.description));
            //  memset(Room.title, 0, sizeof (Room.title));

            //reset all values  
            total_items_required = fgetc(roomfile); //advance to next pointer
            total_items_required ^= (STARTING_ROOM + 122); //decrypt it

            count = 0;
            counter = 0;
            printf("Total items required for this room  %d\n", total_items_required);


            //in case of Room STARTING_ROOM, required item was not found
            /*******************************************************************************/ 
            //if no. of items required  > 0
            //this block of code checks all items required and if they are in player inventory

            if(total_items_required > 0){
              count = total_items_required;
              printf("Total items required for this room  %d\n", total_items_required);
              while(count > 0){
                required_items = fgetc(roomfile);
                required_items ^= (STARTING_ROOM + 122);
                printf("Required Items: %x\n", required_items);
                //check if items required are in inventory
                if(FindInInventory(required_items)== SUCCESS){
                  items_in_inventory++;
                }
                printf("No. of items matched in inventory:  %d\n", items_in_inventory);
                count--;
              }
              /***************************************************************************************/
              //once we check all items required and if they are in inventory, move to Description               
              //check description
              description_length = fgetc(roomfile);
              description_length ^= (STARTING_ROOM + 122); //decrypt it
              printf("Description length is %d\n", description_length);
              fread(Room.description, 1, description_length, roomfile);
              char desc[description_length + 1];


              //decrypt the description into desc
              for (i = 0; i < description_length ; i++){
                desc[i] = Room.description[i] ^ (STARTING_ROOM + 122);
              }

              printf("Room Description: ");
              for (j = 0; j < description_length; j++){
                printf("%c",desc[j]);
              }
              /**********************************************/
              printf("\n");
              // static uint8_t items_contained;
              //  static uint8_t items_contained_in_inventory;
              /******************************************************************************/    
              //Items contained
              total_items_contained_in_roomfile = fgetc(roomfile);
              total_items_contained_in_roomfile ^= (STARTING_ROOM + 122);
              counter = total_items_contained_in_roomfile;
              printf("total_items_contained_in_roomfile is %d\n", total_items_contained_in_roomfile);
              printf("Counter is  %d\n", total_items_contained_in_roomfile);    
              while (counter > 0) {
                items_contained_in_inventory = fgetc(roomfile);
                items_contained_in_inventory^= (STARTING_ROOM + 122);
                AddToInventory(items_contained_in_inventory);
                counter--;
              }
              /******************Get Exits********************/
              Room.north = fgetc(roomfile);
              Room.east = fgetc(roomfile);
              Room.south = fgetc(roomfile);
              Room.west = fgetc(roomfile);

            }
            else if(total_items_required == 0){
              //if no. of items required is 0

              //check description
              description_length = fgetc(roomfile);
              description_length ^= (STARTING_ROOM + 122); //decrypt it
              printf("Description length is %d\n", description_length);
              fread(Room.description, 1, description_length, roomfile);
              char desc[description_length + 1];


              //decrypt the description into desc
              for (i = 0; i < description_length ; i++){
                desc[i] = Room.description[i] ^ (STARTING_ROOM + 122);
              }

              printf("Room Description: ");
              for (j = 0; j < description_length; j++){
                printf("%c",desc[j]);
              }
              printf("\n");

              //Items contained
              total_items_contained_in_roomfile = fgetc(roomfile);
              total_items_contained_in_roomfile ^= (STARTING_ROOM + 122);
              printf("Counter is %d\n", total_items_contained_in_roomfile);
              counter = total_items_contained_in_roomfile;
              printf("total_items_contained_in_roomfile is %d\n", total_items_contained_in_roomfile);
              while (counter > 0) {
                items_contained_in_inventory = fgetc(roomfile);
                items_contained_in_inventory^= (STARTING_ROOM + 122);
                printf("total_items_contained_in_inventory is %d\n", items_contained_in_inventory);
                AddToInventory(items_contained_in_inventory);
                counter--;
              }
              /*********Get Exits*******************/
              Room.north = fgetc(roomfile);
              Room.east = fgetc(roomfile);
              Room.south = fgetc(roomfile);
              Room.west = fgetc(roomfile);


              room_version = TRUE;
              return SUCCESS;
            }
            //if player inventory has all items required, the current room version is to be displayed
            if (items_in_inventory == total_items_required) {
              room_version = TRUE;
            } else {
              //Go to next room version
              fseek(roomfile, 0, SEEK_CUR);
            }
          }

          return SUCCESS;

          }

          return STANDARD_ERROR;
        }

        /**
         * This function sets up anything that needs to happen at the start of the game. This is just
         * setting the current room to STARTING_ROOM and loading it. It should return SUCCESS if it succeeds
         * and STANDARD_ERROR if it doesn't.
         * @return SUCCESS or STANDARD_ERROR
         */
        int GameInit(void){
          CurrentRoom = STARTING_ROOM;
          sprintf(buffer, "/room%d.txt", CurrentRoom);
          roomfile = fopen(buffer, "rb");
          if (roomfile == NULL) {
            FATAL_ERROR();
            return 0;
          }
          GameGoNorth();
          return SUCCESS;
        }

        /**
         * Copies the current room title as a NULL-terminated string into the provided character array.
         * Only a NULL-character is copied if there was an error so that the resultant output string
         * length is 0.
         * @param title A character array to copy the room title into. Should be GAME_MAX_ROOM_TITLE_LENGTH+1
         *             in length in order to allow for all possible titles to be copied into it.
         * @return The length of the string stored into `title`. Note that the actual number of chars
         *         written into `title` will be this value + 1 to account for the NULL terminating
         *         character.
         */
        int GameGetCurrentRoomTitle(char *title){
          int m = 0;
          for(m = 0; m < title_length ; m++){
            Room.title[m] = Room.title[m] ^ (STARTING_ROOM + 122);
          }

          //    Room.title ^= (STARTING_ROOM + 122); //DECRYPT
          strcpy(title, Room.title);
          return title_length;
        }

        /**
         * GetCurrentRoomDescription() copies the description of the current room into the argument desc as
         * a C-style string with a NULL-terminating character. The room description is guaranteed to be less
         * -than-or-equal to GAME_MAX_ROOM_DESC_LENGTH characters, so the provided argument must be at least
         * GAME_MAX_ROOM_DESC_LENGTH + 1 characters long. Only a NULL-character is copied if there was an
         * error so that the resultant output string length is 0.
         * @param desc A character array to copy the room description into.
         * @return The length of the string stored into `desc`. Note that the actual number of chars
         *          written into `desc` will be this value + 1 to account for the NULL terminating
         *          character.
         */
        int GameGetCurrentRoomDescription(char *desc){
          int m = 0;
          for(m = 0; m < description_length ; m++){
            Room.description[m] = Room.description[m] ^ (STARTING_ROOM + 122);
          } strcpy(desc, Room.description);
          return description_length;
        }

        /**
         * This function returns the exits from the current room in the lowest-four bits of the returned
         * uint8 in the order of NORTH, EAST, SOUTH, and WEST such that NORTH is in the MSB and WEST is in
         * the LSB. A bit value of 1 corresponds to there being a valid exit in that direction and a bit
         * value of 0 corresponds to there being no exit in that direction. The GameRoomExitFlags enum
         * provides bit-flags for checking the return value.
         *
         * @see GameRoomExitFlags
         *
         * @return a 4-bit bitfield signifying which exits are available to this room.
         */
        uint8_t GameGetCurrentRoomExits(void){
          uint8_t ExitRoomStatus = 0b0000; //initialize

          if(Room.north != 0){
            ExitRoomStatus |=  GAME_ROOM_EXIT_NORTH_EXISTS;
          }
          if(Room.east != 0){
            ExitRoomStatus |=  GAME_ROOM_EXIT_EAST_EXISTS;
          }
          if(Room.south != 0){
            ExitRoomStatus |=  GAME_ROOM_EXIT_SOUTH_EXISTS;
          }
          if(Room.west != 0){
            ExitRoomStatus |=  GAME_ROOM_EXIT_WEST_EXISTS;
          }

          return ExitRoomStatus;

        }


      


