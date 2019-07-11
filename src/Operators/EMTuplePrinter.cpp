#include "EMTuplePrinter.h"

TuplePrinter::TuplePrinter(Operator* input)
{
	this->input = input;			
}

TuplePrinter::~TuplePrinter()
{
}

void TuplePrinter::printTuples(int nattrs) {
	SBlock* sb = NULL;
	while(true) {
		if (sb == NULL || !sb->hasNext()) {
			sb = input->getNextSValBlock(0); // DSM FIXME 0 assumption
			if (sb == NULL) {
				break;
			}
		}

		SPair* p = sb->getNext();
		int* data = (int*) p->value;

		cout << "p" << p->position;

		for (int j=0; j < nattrs; j++) {
			cout << " " << data[j];
		}
		cout << endl;
	}
}

void TuplePrinter::tossTuples() {
	SBlock* sb = NULL;
	int i = 0;
	cerr << "Tossing!" << endl;
	while(true) {
		if (sb == NULL || !sb->hasNext()) {
			sb = input->getNextSValBlock(0); // DSM FIXME 0 assumption
			if (sb == NULL) {
				break;
			}
		}
		/*SPair* p = */sb->getNext();
		if (i%10000000 == 0) {
			cerr << i << endl;
		}
		i++;
	}
}
