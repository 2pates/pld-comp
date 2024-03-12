public class Example {
	public static void putdigit(int digit) {
		byte c = (byte)(digit + '0');
		System.out.print((char)Byte.toUnsignedInt(c));
	}

	public static void main(String[] args) {
		for(int x = 0; x < 10; x++) {
			putdigit(x);
		}
		System.out.print('\n');

		System.exit(42);
	}
}
