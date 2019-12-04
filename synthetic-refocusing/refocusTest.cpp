#include "refocusing.hpp"

using namespace std;

int main()
{
    Refocusing syntheticRefocus;

    cout << "Criei o objeto" << endl;
    syntheticRefocus.LoadDepthImage("Sherlock_Refined.bmp");
    cout << "Li a profundidade" << endl;
    syntheticRefocus.LoadReferenceImage("Sherlock_ref.bmp");
    cout << "Li a imagem de referencia" << endl;
    syntheticRefocus.Refocus(REFOCUSING_THRESHOLD, 80);
    cout << "Fiz o refocus" << endl;

    cv::waitKey(0);
}