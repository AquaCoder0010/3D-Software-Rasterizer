#include "renderer.hpp"
#include <unistd.h>

#include <iostream>


int main() {
  BufferWindow c = create(60, 20, ' ');

  std::string message = "Hello terminal!";
  std::string current_message = "";
  int count = 0;
  while (count <= static_cast<int>(message.length()) ) {
    current_message += message[count];
	
    clear_window(&c, ' ');
	draw_text(&c, 20, 10, current_message.c_str());
    render(&c);
  
	sleep(1);
    count += 1;
  }
  destroy(&c);
  
  return 0;
}

