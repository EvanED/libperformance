#include <unistd.h>

extern "C" void annotate(const char * str);

int main()
{
    char * c[12] = {nullptr};
    for(int i=0; i<12; ++i) {
        sleep(10);
        if(i<6 && i%2 == 0) {
            annotate("allocate");
            c[i] = new char[1000*1000];
        }
        else if(i%2 == 0) {
            annotate("allocate and initialize");
            c[i] = new char[1000*1000]();
        }
    }
    for(int i=0; i<12; ++i) {
        sleep(10);
        annotate("deallocate");
        delete[] c[i];
    }
}
