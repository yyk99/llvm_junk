//
//
//

int arr[10];

int foo(int k)
{
  return arr[k];
}

int bar(int k)
{
  int arr[10]; // = {1,2,3,4,5,6,7,8,9,0};

  return arr[k];
}
