#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#include <string>
#include <cctype>

using namespace Gecode;

namespace {
	extern const int board[9][9];
	//This function is used to input Sudoku instance.
	int sudokuField(const int s[9][9], int i, int j);
}

class Sudoku : public Script {

protected:
	IntVarArray x;

public:
	Sudoku(const SizeOptions& opt) : x(*this, 9 * 9, 1, 9) {

		Matrix<IntVarArray> m(x, 9, 9);

		for (int i = 0; i < 9; i++) {
			distinct(*this, m.row(i), opt.icl());
			distinct(*this, m.col(i), opt.icl());
		}

		for (int i = 0; i < 9; i += 3) {
			for (int j = 0; j < 9; j += 3) {
				distinct(*this, m.slice(i, i + 3, j, j + 3), opt.icl());
			}
		}

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (int v = sudokuField(board, i, j)) {
					//Here the m(i, j) is the element in colomn i and row j.
					rel(*this, m(i, j), IRT_EQ, v);
				}
			}
		}

		branch(*this, x, INT_VAR_NONE(), INT_VAL_SPLIT_MIN());
	}

	Sudoku(bool share, Sudoku& s) : Script(share, s) {
		x.update(*this, share, s.x);
	}

	virtual Space* copy(bool share) {
		return new Sudoku(share, *this);
	}

	virtual void print(std::ostream& os) const {
		os << "  ";
		for (int i = 0; i < 81; i++) {
			os << x[i] << " ";
			if ((i + 1) % 9 == 0)
				os << std::endl << "  ";
		}
	}

};

int main(int argc, char* argv[]) {

	SizeOptions opt("Sudoku");
	//opt.size(1);
	// repeat 10 times to calculate average executing time
	opt.iterations(10);
	//set Integer consistency level
	opt.icl(ICL_DOM);
	// search for the first solution
	opt.solutions(1);
	opt.parse(argc, argv);

	Script::run<Sudoku, DFS, SizeOptions>(opt);

	return 0;
}

namespace {

	// The value 0 represents that the cell is empty.
	const int board[9][9] = {
		{ 0, 0, 0, 2, 0, 5, 0, 0, 0 },
		{ 0, 9, 0, 0, 0, 0, 7, 3, 0 },
		{ 0, 0, 2, 0, 0, 9, 0, 6, 0 },

		{ 2, 0, 0, 0, 0, 0, 4, 0, 9 },
		{ 0, 0, 0, 0, 7, 0, 0, 0, 0 },
		{ 6, 0, 9, 0, 0, 0, 0, 0, 1 },

		{ 0, 8, 0, 4, 0, 0, 1, 0, 0 },
		{ 0, 6, 3, 0, 0, 0, 0, 8, 0 },
		{ 0, 0, 0, 6, 0, 8, 0, 0, 0 }
	};

	int sudokuField(const int s[9][9], int i, int j) {

		// In Gecode, m(i, j) is the element in column i and row j,
		// so here s(j, i) <-> m(i, j).
		int c = s[j][i];

		return c;
	}
}