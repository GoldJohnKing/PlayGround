#include "DpiScaling.h"

#include <iostream>
using namespace std;

int main()
{
	int dpiScaleLevel = 0;
	cout << "Enter DPI Scale Level:" << endl;

	cin >> dpiScaleLevel;
	SetDpiScaling(dpiScaleLevel);

	system("pause");

	return 0;
}
