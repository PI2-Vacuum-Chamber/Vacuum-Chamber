#include <iostream> 
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace std; 

struct Item{ int id, tipo, valor; }; 

int main() { 
  vector<Item> containeritens;

  ifstream ifs("receiving.json");
  Json::Reader reader; 
  Json::Value obj;

  reader.parse(ifs, obj); 

  const Json::Value& itens = obj["Itens"]; 

  for (int i = 0; i < itens.size(); i++) {
    Item item;
    item.id = itens[i]["id"].asUInt();
    item.tipo = itens[i]["tipo"].asUInt();
    item.valor = itens[i]["valor"].asUInt();

    contairneritens.pushback(item);
  }
  return 0; 
} 
