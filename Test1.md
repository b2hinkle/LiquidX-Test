
# Test1

The ``PrintArray`` function shows some bad practices as well as some areas that can be improved upon. The function's parameter should be a const reference. Not passing by reference will cost performance since TArrays hold lots of data and it will all have to be copied into a newly allocated TArray when the function is called. The ``const`` keyword will ensure that the TArray can't be modified within the function's scope. The ``auto`` keyword can also be a const reference so that each iteration doesn't cost a pointless pointer copy.
I would also make ``PrintArray`` a const function to ensure that there are no side effects from calling it. The function can also be made static since its logic only depends on the supplied data.
If all changes listed are made it would look like this:
```cpp
static void PrintArray(const TArray<AActor*>& Array) const
{
	for(const auto& Actor : Array)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor's name is: %s"),
		*Actor->GetName());
	}
}
```
Also a log verbosity of ``Warning`` may not be appropriate for a log like this, and the log could also be improved with a specific category.