	Dr.Dobb's
	September 28, 2009
	C Snippet #6

	Bob Stout
### Getting a string of arbitrary length

Q: HOW DO I... get a string of any arbitrary length from the console?

A: The ANSI C gets() function is one of those obsolescent functions that is in the standard simply because it would have broken too much existing code to have left it out. The ANSI function fgets() is generally preferable to gets() since it protects against buffer overflow.

The following getstring() function combines the best of both, using dynamic memory allocation to get as large an input string as the user cares to type in. Error trapping is built in as shown. As with all functions that use dynamic memory allocation, once you're through with the returned string, it's a good idea to free() it and return the memory used to the heap.

