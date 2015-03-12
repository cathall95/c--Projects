char encrypt_char(char c) {
	char a;
	
	if (c >= 'a' && c <= 'z') {
		a = 'a' + ('z' - c);
	} else if (c >= 'A' && c <= 'Z') {
		a = 'A' + ('Z' - c);
	} else if (c >= '0' && c <= '9') {
		a  = '0' + ('9' - c);
	} else {
		a = c;
	}

	return a;
}
