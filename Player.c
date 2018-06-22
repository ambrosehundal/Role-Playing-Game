#include "Player.h"
#include <stdint.h>
#include <stdio.h>
// Define how big the player's inventory is.
#define INVENTORY_SIZE 4

uint8_t inventory[INVENTORY_SIZE] = {'\0'};

/**
 * Adds the specified item to the player's inventory if the inventory isn't full.
 * @param item The item number to be stored: valid values are 0-255.
 * @return SUCCESS if the item was added, STANDARD_ERRROR if the item couldn't be added.
 */
int AddToInventory(uint8_t item){
//  int i;


  if(inventory[0] == '\0'){
    inventory[0] = item;
    printf("Inventory item[0] is:%d\n", inventory[0]);
    return SUCCESS;
  }
  else if ((inventory[1] == '\0') &&
      (inventory[0] != item)) {
    inventory[1] = item;
    printf("Inventory item[1] is:%d\n", inventory[1]);
    return SUCCESS;
  } 

  else if ((inventory[2] == '\0') &&
      (inventory[1] != item) &&
      (inventory[0] != item)) {
    inventory[2] = item;
    printf("Inventory item[2] is:%d\n", inventory[2]);
    return SUCCESS;

  } else if ((inventory[3] == '\0') &&
      (inventory[2] != item) &&
      (inventory[1] != item) &&
      (inventory[0] != item)) {
    inventory[3] = item;
    printf("Inventory item[3] is:%d\n", inventory[3]);

    return SUCCESS;

  }

  printf("Error! cannot add item to inventory\n");
  return STANDARD_ERROR;
}




/**
 * Check if the given item exists in the player's inventory.
 * @param item The number of the item to be searched for: valid values are 0-255.
 * @return SUCCESS if it was found or STANDARD_ERROR if it wasn't.
 */
int FindInInventory(uint8_t item){
  int i;
  for(i = 0; i < INVENTORY_SIZE; i++){
    if(inventory[i] == item){
//      printf("yes it exists");
      return SUCCESS;
    } 
  }
  printf("Item not found in inventory\n");
  return STANDARD_ERROR;



}

