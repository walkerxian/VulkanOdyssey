
#include "soc_app_base.hpp"

//std

#include <stdlib.h>
#include <iostream>
#include <stdexcept>


int main() {
  
  soc::SocAppBase app{};

  try{

    app.run();

  }catch(const std::exception &e){

    std::cerr<<e.what() << '\n';
    return EXIT_FAILURE;
  } 

  return EXIT_SUCCESS; 
}
