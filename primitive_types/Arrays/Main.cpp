#include <iostream>

using namespace std;

int main(){
	// without initialization
	int array1[10];

	cout << array1[5] << endl;
	cout << array1[100] << endl << endl;

	//int a{ 5 };
	//int array1[a]; // error - must be const value

	// with initialization
	double array2[5]{ 1, 2.0, 3.0, 4.0, 5.0 };

	cout << array2[1] << endl;
	cout << array2[100] << endl << endl;

	// with initialization
	double array3[]{ 1, 2.0, 3.0, 4.0, 5.0 };

	// assign to pointer
	double *arrPtr1{ nullptr };
	double array4[3]{ 1.0, 2.0, 3.0 };

	arrPtr1 = array4;
	
	cout << arrPtr1[1] << endl << endl;

	// pointer array
	double* arrPtr2{ new double[10] };
	delete[] arrPtr2;
	

	int a;
	cin >> a;

	return 0;
}