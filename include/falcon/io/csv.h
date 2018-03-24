#include <string>
#include <vector>

template <class OStream> class Csv {
  OStream &os_;

public:
  Csv(OStream &os) : os_(os) {}
  ~Csv() { os_.flush(); }

  template <class Data> void writeRowVec(const std::vector<Data> row) {
    os_ << row.at(0);
    for (size_t i = 1; i < row.size(); ++i) {
      os_ << ',';
      os_ << row.at(i);
    }
    os_ << '\n';
  }

  template <class Arg, class... Args>
  void writeRow(const Arg &first, Args... args) {
    os_ << first;
    os_ << ',';
    writeRow(args...);
  }

  template <class Arg> void writeRow(const Arg &first) {
    os_ << first;
    os_ << '\n';
  }
};
