#include "../../cpp/Time.hpp"
#include "../../cpp/Io.hpp"

int main()
{
    hsd::time t1;
    hsd::io::print<"Today we are in {}">(t1.to_text());

    for(hsd::u32 i = 0; i < -1; i++);
    t1.update();

    hsd::io::print<"Today we are in {}">(t1.to_text());
}