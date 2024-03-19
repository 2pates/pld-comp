int main() { // 1
    int a;

    { // 11
        a = 5;
		int d = b;
        int c = 12;
    }

    { // 12
        d = 5; 
    }
	return a;
}
