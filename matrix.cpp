#include<iostream>
#include<vector>
#include<string>
#include<cassert>
#include <stdexcept>
#include <cstdint>

class BigInteger {
private:
    static const long long max_length_ = 1;
    static const long long max_number_ = 10;
    std::vector<long long> big_number_;
    bool positive = true;

    friend class Rational;

    void trueLength();
    void changeSignInt();
    void clear();
public:
    BigInteger();
    BigInteger(int integer);

    explicit operator bool() const;

    ~BigInteger();

    BigInteger(const BigInteger& number);

    BigInteger& operator+=(const BigInteger& number);
    BigInteger& operator-=(const BigInteger& number);
    BigInteger& operator*=(const BigInteger& number);
    BigInteger& operator/=(const BigInteger& number);
    BigInteger& operator%=(const BigInteger& number);

    void swap(BigInteger& number);
    BigInteger& operator=(BigInteger number);

    BigInteger& operator++();
    BigInteger& operator--();

    BigInteger operator++(int);
    BigInteger operator--(int);

    std::string toString() const;

    friend bool operator<(const BigInteger& base, const BigInteger& number);

    friend std::ostream& operator<<(std::ostream& out, const BigInteger& number);
    friend std::istream& operator>>(std::istream& in, BigInteger& number);

    void changeSign();
};

BigInteger::BigInteger() : BigInteger(0) {}

BigInteger::BigInteger(const BigInteger& number) {
    big_number_ = number.big_number_;
    positive = number.positive;
}

BigInteger::BigInteger(int integer) {
    positive = true;
    if (integer < 0) {
        positive = false;
        integer *= -1;
    }
    if (integer == 0)
        big_number_.push_back(0);
    else {
        while (integer > 0) {
            big_number_.push_back(integer % max_number_);
            integer /= max_number_;
        }
    }
}

BigInteger::~BigInteger() {
    positive = true;
    big_number_.clear();
}

BigInteger::operator bool() const {
    return !(big_number_.size() == 1 && big_number_[0] == 0);
}


bool operator>(const BigInteger& base, const BigInteger& number) {
    return number < base;
}

bool operator==(const BigInteger& base, const BigInteger& number) {
    return !(base < number) && !(number < base);
}

bool operator>=(const BigInteger& base, const BigInteger& number) {
    return  (base == number) || (base > number);
}

bool operator<=(const BigInteger& base, const BigInteger& number) {
    return (base == number) || (base < number);
}

bool operator!=(const BigInteger& base, const BigInteger& number) {
    return !(base == number);
}

void BigInteger::clear() {
    positive = true;
    big_number_.clear();
}

void BigInteger::swap(BigInteger& number) {
    std::swap(big_number_, number.big_number_);
    std::swap(positive, number.positive);
}

BigInteger& BigInteger::operator=(BigInteger number) {
    swap(number);
    return *this;
}

void BigInteger::changeSign() {
    if (!(big_number_.size() == 1 && big_number_[0] == 0))
        positive = !positive;
}

BigInteger operator-(BigInteger& number) {
    BigInteger new_int = number;
    new_int.changeSign();
    return new_int;
}

void BigInteger::changeSignInt() {
    for (size_t i = 0; i < big_number_.size(); ++i) {
        big_number_[i] *= -1;
    }
    positive = !positive;
    for (size_t i = 0; i < big_number_.size(); ++i) {
        if (big_number_[i] >= max_number_) {
            long long perenos = big_number_[i] / max_number_;
            if (i < big_number_.size() - 1) {
                big_number_[i + 1] += perenos;
            }
            else {
                big_number_.push_back(perenos);
            }
            big_number_[i] %= max_number_;
        }
        else if (big_number_[i] < 0) {
            long long perenos = big_number_[i];
            perenos /= max_number_;
            perenos *= -1;
            ++perenos;
            big_number_[i] += (perenos * max_number_);
            big_number_[i + 1] -= perenos;
            if (big_number_[i] == max_number_) {
                big_number_[i] -= max_number_;
                ++big_number_[i + 1];
            }
        }
    }
    while (big_number_.size() > 0 && big_number_[(big_number_.size() - 1)] == 0) {
        big_number_.pop_back();
    }
    if (big_number_.size() == 0) {
        big_number_.push_back(0);
        positive = true;
    }
}

void BigInteger::trueLength() {
    for (size_t i = 0; i < big_number_.size(); ++i) {
        if (big_number_[i] >= max_number_) {
            long long perenos = big_number_[i] / max_number_;
            if (i < big_number_.size() - 1) {
                big_number_[i + 1] += perenos;
            }
            else {
                big_number_.push_back(perenos);
            }
            big_number_[i] %= max_number_;
        }
        else if (big_number_[i] < 0) {
            if (i == big_number_.size() - 1) {
                changeSignInt();
                return;
            }
            long long perenos = big_number_[i];
            perenos /= max_number_;
            perenos *= -1;
            ++perenos;
            big_number_[i] += (perenos * max_number_);
            big_number_[i + 1] -= perenos;
            if (big_number_[i] == max_number_) {
                big_number_[i] -= max_number_;
                ++big_number_[i + 1];
            }
        }
    }
    while (big_number_.size() > 0 && big_number_[(big_number_.size() - 1)] == 0) {
        big_number_.pop_back();
    }
    if (big_number_.size() == 0) {
        big_number_.push_back(0);
        positive = true;
        return;
    }
}

BigInteger& BigInteger::operator+=(const BigInteger& number) {
    if (big_number_.size() < number.big_number_.size())
        big_number_.resize(number.big_number_.size());
    for (size_t i = 0; i < number.big_number_.size(); ++i) {
        if (positive == number.positive) {
            big_number_[i] += number.big_number_[i];
        }
        else {
            big_number_[i] -= number.big_number_[i];
        }
    }
    trueLength();
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& number) {
    if (big_number_.size() < number.big_number_.size())
        big_number_.resize(number.big_number_.size());
    for (size_t i = 0; i < number.big_number_.size(); ++i) {
        if (positive == !number.positive || !positive == number.positive) {
            big_number_[i] += number.big_number_[i];
        }
        else {
            big_number_[i] -= number.big_number_[i];
        }
    }
    trueLength();
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& number) {
    if (!number.positive)
        positive = !positive;
    big_number_.resize(big_number_.size() + number.big_number_.size());
    for (size_t i = big_number_.size() - number.big_number_.size(); i > 0; --i) {
        --i;
        for (size_t j = number.big_number_.size(); j > 0; --j) {
            --j;
            if (j != 0 && number.big_number_[j] != 0) {
                big_number_[i] *= number.big_number_[j];
                big_number_[i + j] += big_number_[i];
                big_number_[i] /= number.big_number_[j];
            }
            if (j == 0) {
                big_number_[i] *= number.big_number_[0];
            }
            ++j;
        }
        ++i;
    }
    trueLength();
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& number) {
	if (number == BigInteger(0))
		throw std::runtime_error("Division by 0");
    if (big_number_.size() == 1 && big_number_[0] == 0)
        return *this;
    if (!number.positive)
        positive = !positive;
    BigInteger positive_int = (*this);
    if (!positive_int.positive)
        positive_int.changeSign();
    std::string string = positive_int.toString();
    BigInteger answer = 0;
    BigInteger current = 0;
    size_t string_start = 0;
    size_t divisor_length = (number.big_number_.size() - 1) * max_length_;
    long long counter = number.big_number_[(number.big_number_.size() - 1)];
    while (counter > 0) {
        counter /= 10;
        ++divisor_length;
    }
    if (divisor_length > string.size()) {
        *this *= 0;
        return *this;
    }
    for (size_t i = 0; i < divisor_length; ++i) {
        current *= 10;
        current += static_cast <long long> (string[string_start] - '0');
        ++string_start;
    }
    BigInteger temporary_base = number;
    if (!temporary_base.positive)
        temporary_base.changeSign();
    BigInteger temporary_working = number;
    if (!temporary_working.positive)
        temporary_working.changeSign();
    long long times = 10;
    temporary_working *= 10;
    while (temporary_working > current) {
        temporary_working -= temporary_base;
        --times;
    }
    current -= temporary_working;
    answer *= 10;
    answer += times;
    times = 10;
    temporary_working = temporary_base;
    temporary_working *= 10;
    while (string_start < string.size()) {
        current *= 10;
        current += static_cast <long long> (string[string_start] - '0');
        while (temporary_working > current) {
            temporary_working -= temporary_base;
            --times;
        }
        current -= temporary_working;
        answer *= 10;
        answer += times;
        times = 10;
        temporary_working = temporary_base;
        temporary_working *= 10;
        ++string_start;
    }
    big_number_ = answer.big_number_;
    trueLength();
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& number) {
    BigInteger this_integer = *this;
    if (!this_integer.positive)
        this_integer.changeSign();
    BigInteger number_integer = number;
    if (!number_integer.positive)
        number_integer.changeSign();
    this_integer /= number_integer;
    this_integer *= number_integer;
    if (positive) {
        *this -= this_integer;
    }
    else {
        *this += this_integer;
    }
    trueLength();
    return *this;
}

BigInteger& BigInteger::operator++() {
    *this += 1;
    trueLength();
    return *this;
}

BigInteger& BigInteger::operator--() {
    *this -= 1;
    trueLength();
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger answer = (*this);
    ++(*this);
    return answer;
}

BigInteger BigInteger::operator--(int) {
    BigInteger answer = (*this);
    --(*this);
    return answer;
}

bool operator<(const BigInteger& base, const BigInteger& number) {
    if (base.positive && !number.positive)
        return false;
    if (!base.positive && number.positive)
        return true;
    if (base.big_number_.size() != number.big_number_.size()) {
        if (base.big_number_.size() < number.big_number_.size())
            return base.positive;
        else
            return !base.positive;
    }
    for (int i = number.big_number_.size() - 1; i >= 0; --i) {
        if (base.big_number_[i] < number.big_number_[i]) {
            return base.positive;
        }
        if (base.big_number_[i] > number.big_number_[i]) {
            return !base.positive;

        }
    }
    return false;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& number) {
    out << number.toString();
    return out;
}

std::istream& operator>>(std::istream& in, BigInteger& number) {
    std::string input;
    in >> input;
    number.big_number_.clear();
    number.positive = true;
    size_t read_from = 0;
    if (input[0] == '-') {
        number.positive = false;
        read_from = 1;
    }
    if (input[0] == '+') {
        read_from = 1;
    }
    size_t last_number = (input.size() - 1);
    if (read_from == last_number && input[read_from] == 0) {
        number.big_number_.push_back(0);
        number.positive = true;
        return in;
    }
    for (size_t i = input.size(); i > read_from; i -= number.max_length_) {
        if ((i - read_from) >= number.max_length_) {
            int integer_to_push = atoi((input.substr(i - number.max_length_, number.max_length_)).c_str());
            number.big_number_.push_back(static_cast <long long> (integer_to_push));
        }
        else {
            int integer_to_push = atoi((input.substr(read_from, i - read_from)).c_str());
            number.big_number_.push_back(static_cast <long long> (integer_to_push));
            break;
        }
    }
    number.trueLength();
    return in;
}

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
    BigInteger answer = first;
    answer += second;
    return answer;
}

BigInteger operator-(const BigInteger& first, const BigInteger& second) {
    BigInteger answer = first;
    answer -= second;
    return answer;
}

BigInteger operator*(const BigInteger& first, const BigInteger& second) {
    BigInteger answer = first;
    answer *= second;
    return answer;
}


BigInteger operator/(const BigInteger& first, const BigInteger& second) {
    BigInteger answer = first;
    answer /= second;
    return answer;
}


BigInteger operator%(const BigInteger& first, const BigInteger& second) {
    BigInteger answer = first;
    answer %= second;
    return answer;
}

std::string BigInteger::toString() const {
    std::string string;
    if (!positive) {
        string.push_back('-');
    }
    string += std::to_string(big_number_[big_number_.size() - 1]);
    for (size_t i = (big_number_.size() - 1); i > 0; --i) {
        --i;
        std::string substring = std::to_string(big_number_[i]);
        for (size_t j = substring.size(); j < max_length_; ++j) {
            string.push_back('0');
        }
        string += substring;
        ++i;
    }
    return string;
}

class Rational {
private:
    BigInteger numerator_;
    BigInteger denominator_;

    void trueLength();
public:
    Rational();
    Rational(int number);
    Rational(const BigInteger& number);
    Rational(const BigInteger& num, const BigInteger& den);

    ~Rational();

    explicit operator double() const;

    void swap(Rational& number);
    Rational& operator=(Rational number);

    Rational& operator+=(const Rational& number);
    Rational& operator-=(const Rational& number);
    Rational& operator*=(const Rational& number);
    Rational& operator/=(const Rational& number);

    std::string toString() const;

    std::string asDecimal(const size_t precision = 0) const;

    friend bool operator<(const Rational& first, const Rational& second);

    void changeSign();
};

Rational::Rational() : numerator_(BigInteger(0)), denominator_(BigInteger(1)) {  }

Rational::Rational(int number) : numerator_(BigInteger(number)), denominator_(BigInteger(1)) {  }

Rational::Rational(const BigInteger& number) : numerator_(number), denominator_(BigInteger(1)) { }

Rational::Rational(const BigInteger& n, const BigInteger& d) : numerator_(n), denominator_(d) { }

void Rational::swap(Rational& number) {
    std::swap(numerator_, number.numerator_);
    std::swap(denominator_, number.denominator_);
}

Rational& Rational::operator=(Rational number) {
    swap(number);
    return *this;
}

Rational::~Rational() {
    numerator_.clear();
    denominator_.clear();
}

Rational::operator double() const {
    double answer = 0;
    std::string string = asDecimal(18);
    size_t counter = 0;
    for (size_t i = 0; i < string.size(); ++i) {
        if (string[i] == '.') {
            counter = i;
            break;
        }
        answer *= 10;
        answer += static_cast <double> (string[i] - '0');
    }
    if (counter == 0)
        return answer;
    double d = 1;
    for (size_t i = (counter + 1); i < string.size(); ++i) {
        d *= 10;
        answer += (static_cast <double> (string[i] - '0')) / d;
    }
    return answer;
}

void Rational::trueLength() {
    if (!denominator_.positive) {
        numerator_.changeSign();
        denominator_.positive = true;
    }
    BigInteger n = numerator_;
    BigInteger d = denominator_;
    BigInteger gcd;
    while (true) {
        n.positive = true;
        d.positive = true;
        if (n == 0) {
            gcd = d;
            break;
        }
        if (d == 0) {
            gcd = n;
            break;
        }
        if (d > n) {
            d %= n;
        }
        else {
            gcd = (n % d);
            n = d;
            d = gcd;
        }
    }
    numerator_ /= gcd;
    denominator_ /= gcd;
}

Rational& Rational::operator+=(const Rational& number) {
    numerator_ *= number.denominator_;
    numerator_ += denominator_ * number.numerator_;
    denominator_ *= number.denominator_;
    trueLength();
    return *this;
}

Rational& Rational::operator-=(const Rational& number) {
    numerator_ *= number.denominator_;
    numerator_ -= denominator_ * number.numerator_;
    denominator_ *= number.denominator_;
    trueLength();
    return *this;
}

Rational& Rational::operator*=(const Rational& number) {
    numerator_ *= number.numerator_;
    denominator_ *= number.denominator_;
    trueLength();
    return *this;
}

Rational& Rational::operator/=(const Rational& number) {
    numerator_ *= number.denominator_;
    denominator_ *= number.numerator_;
    trueLength();
    return *this;
}

void Rational::changeSign() {
    numerator_.changeSign();
}

Rational operator-(const Rational& number) {
    Rational answer = number;
    answer.changeSign();
    return answer;
}

std::string Rational::toString() const {
    std::string string;
    string += numerator_.toString();
    if (denominator_ == 1)
        return string;
    string += '/';
    string += denominator_.toString();
    return string;
}

std::string Rational::asDecimal(const size_t precision) const {
    std::string string;
    if (numerator_ == 0) {
        string.push_back('0');
        if (precision != 0) {
            string.push_back('.');
        }
        for (size_t i = 0; i < precision; ++i) {
            string.push_back('0');
        }
        return string;
    }
    BigInteger n = numerator_;
    if (!n.positive) {
        string.push_back('-');
        n.positive = true;
    }
    for (size_t i = 0; i < precision; ++i) {
        n *= 10;
    }
    n /= denominator_;
    std::string number_to_string = n.toString();
    if (number_to_string.size() < precision) {
        string.push_back('0');
        string.push_back('.');
        for (size_t i = 0; i < precision - number_to_string.size(); ++i) {
            string.push_back('0');
        }
        string += number_to_string;
    }
    else {
        for (size_t i = 0; i < number_to_string.size() - precision; ++i) {
            string.push_back(number_to_string[i]);
        }
        string.push_back('.');
        for (size_t i = number_to_string.size() - precision; i < number_to_string.size(); ++i) {
            string.push_back(number_to_string[i]);
        }
    }
    return string;
}

bool operator<(const Rational& first, const Rational& second) {
    return first.numerator_ * second.denominator_ < first.denominator_* second.numerator_;
}

bool operator>(const Rational& first, const Rational& second) {
    return second < first;
}

bool operator>=(const Rational& first, const Rational& second) {
    return !(first < second);
}

bool operator<=(const Rational& first, const Rational& second) {
    return !(first > second);
}

bool operator==(const Rational& first, const Rational& second) {
    return !(first < second || first > second);
}

bool operator!=(const Rational& first, const Rational& second) {
    return (first < second || first > second);
}

Rational operator+(const Rational& first, const Rational& second) {
    Rational answer = first;
    answer += second;
    return answer;
}

Rational operator-(const Rational& first, const Rational& second) {
    Rational answer = first;
    answer -= second;
    return answer;
}

Rational operator*(const Rational& first, const Rational& second) {
    Rational answer = first;
    answer *= second;
    return answer;
}

Rational operator/(const Rational& first, const Rational& second) {
    Rational answer = first;
    answer /= second;
    return answer;
}


template <uint64_t N>
class Finite {
private:
    BigInteger remainder;
    BigInteger n;

    void normalaizer();
    void swap(Finite<N>& number);
public:
    explicit operator bool() const {
        return remainder != BigInteger(0);
    }

    Finite();
    Finite(int input);

    Finite<N>& operator=(const Finite<N>& number);

    ~Finite() = default;

    bool operator==(const Finite<N>& another) const;
    bool operator!=(const Finite<N>& another) const;

    Finite<N>& operator+=(const Finite<N>& another);
    Finite<N>& operator-=(const Finite<N>& another);
    Finite<N>& operator*=(const Finite<N>& another);
    Finite<N>& operator/=(const Finite<N>& another);

    Finite<N>& operator++();
    Finite<N>& operator--();

    friend std::ostream& operator<<(std::ostream& out, const Finite<N>& number) {
        out << number.remainder;
        return out;
    }

    explicit operator double() const {
            return std::stod(remainder.toString());
    }
};

template <uint64_t N>
Finite<N>::Finite() : remainder(0), n(BigInteger(std::to_string(N))) {}

template <uint64_t N>
Finite<N>::Finite(int input) : remainder(input), n(BigInteger(std::to_string(N))) {
    if (remainder < 0)
        remainder -= (remainder / BigInteger(std::to_string(N)) - 1) * BigInteger(std::to_string(N));
    remainder %= BigInteger(std::to_string(N));
}

template <uint64_t N>
Finite<N>& Finite<N>::operator=(const Finite<N>& number) {
    remainder = number.remainder;
    return *this;
}

template <uint64_t N>
bool Finite<N>::operator==(const Finite<N>& another) const {
    return remainder == another.remainder;
}

template <uint64_t N>
bool Finite<N>::operator!=(const Finite<N>& another) const {
    return remainder != another.remainder;
}

template <uint64_t N>
void Finite<N>::normalaizer() {
    if (remainder < 0)
        remainder -= (remainder / BigInteger(std::to_string(N)) - 1) * BigInteger(std::to_string(N));
    remainder %= BigInteger(std::to_string(N));
}

template <uint64_t N>
Finite<N>& Finite<N>::operator++() {
    *this += 1;
    return *this;
}

template <uint64_t N>
Finite<N>& Finite<N>::operator--() {
    *this -= 1;
    return *this;
}

template <uint64_t N>
Finite<N>& Finite<N>::operator+=(const Finite<N>& another) {
    remainder += another.remainder;
    normalaizer();
    return *this;
}

template <uint64_t N>
Finite<N>& Finite<N>::operator-=(const Finite<N>& another) {
    remainder -= another.remainder;
    normalaizer();
    return *this;
}

template <uint64_t N>
Finite<N>& Finite<N>::operator*=(const Finite<N>& another) {
    remainder *= another.remainder;
    normalaizer();
    return *this;
}

BigInteger FastPower(BigInteger a, uint64_t p, BigInteger n) {
    if (p == 0)
        return BigInteger(1);
    if (p % 2 == 1) {
        BigInteger b = FastPower(a, p - 1, n) * a;
        b %= n;
        return b;
    }
    else {
        BigInteger b = FastPower(a, p / 2, n);
        b = (b * b) % n;
        return b;
    }
}

template <uint64_t N>
Finite<N>& Finite<N>::operator/=(const Finite<N>& another) {
    assert(another.remainder != 0);
    if (remainder == 0)
        return *this;
    remainder = remainder * FastPower(another.remainder, N - 2, n);
    remainder %= n;
    return *this;
}

template <uint64_t N>
Finite<N> operator+(const Finite<N>& first, const Finite<N>& second) {
    Finite<N> answer = first;
    answer += second;
    return answer;
}

template <uint64_t N>
Finite<N> operator-(const Finite<N>& first, const Finite<N>& second) {
    Finite<N> answer = first;
    answer -= second;
    return answer;
}

template <uint64_t N>
Finite<N> operator*(const Finite<N>& first, const Finite<N>& second) {
    Finite<N> answer = first;
    answer *= second;
    return answer;
}

template <uint64_t N>
Finite<N> operator/(const Finite<N>& first, const Finite<N>& second) {
    Finite<N> answer = first;
    answer /= second;
    return answer;
}

template <size_t M, size_t N, typename Field = Rational>
class String;

template <size_t M, size_t N, typename Field = Rational>
class CString;

template <size_t M, size_t N, typename Field = Rational>
class Matrix {
private:
    std::vector<std::vector<Field>> Body;

    friend class String<M, N, Field>;
    friend class CString<M, N, Field>;
    std::pair<Matrix<M, N, Field>, bool> gaus() const;
public:
    Matrix();
    Matrix(bool);
    template <typename T>
    Matrix(const std::vector<std::vector<T>>&);

    ~Matrix() = default;

    Matrix<M, N, Field>& operator=(const Matrix<M, N, Field>& number);

    template <size_t M1, size_t N1>
    bool operator==(const Matrix<M1, N1, Field>&) const;
    template <size_t M1, size_t N1>
    bool operator!=(const Matrix<M1, N1, Field>&) const;

    Matrix<M, N, Field> operator+=(const Matrix<M, N, Field>&);
    Matrix<M, N, Field> operator-=(const Matrix<M, N, Field>&);
    Matrix<M, N, Field> operator*=(Field);

    String<M, N, Field> operator[](size_t index);
    const CString<M, N, Field> operator[](size_t index) const;

    Matrix<N, M, Field> transposed() const;
    size_t rank() const;
    std::vector<Field> getRow(size_t) const;
    std::vector<Field> getColumn(size_t) const;

    Field trace();
    Field det();
    void invert();
    Matrix<M, N, Field> inverted() const;
    Matrix<M, N, Field>& operator*=(const Matrix<M, N, Field>&);
};

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix() {
    Body.resize(M);
    for (size_t i = 0; i < M; ++i) {
        Body[i].resize(N);
    }
    if (M == N) {
        for (size_t i = 0; i < N; ++i) {
            Body[i][i] = Field(1);
        }
    }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix(bool) {
    Body.resize(M);
    for (size_t i = 0; i < M; ++i) {
        Body[i].resize(N);
    }
}

template <size_t M, size_t N, typename Field>
template <typename T>
Matrix<M, N, Field>::Matrix(const std::vector<std::vector<T>>& Array) {
    Body.resize(M);
    for (size_t i = 0; i < M; ++i) {
        Body[i].resize(N);
    }
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Body[i][j] = Array[i][j];
        }
    }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator=(const Matrix<M, N, Field>& number) {
    Body = number.Body;
    return *this;
}

template <size_t M, size_t N, typename Field>
std::pair<Matrix<M, N, Field>, bool> Matrix<M, N, Field>::gaus() const {
    Matrix<M, N, Field> Gaus = *this;
    bool true_sign = true;
    size_t start = 0;
    for (size_t j = 0; j < N; ++j) {
        size_t begin = start;
        for (; begin < M; ++begin) {
            if (Gaus.Body[begin][j] != 0)
                break;
        }
        if (begin < M) {
            if (begin != start) {
                for (size_t i = 0; i < N; ++i) {
                    Field swap = Gaus.Body[start][i];
                    Gaus.Body[start][i] = Gaus.Body[begin][i];
                    Gaus.Body[begin][i] = swap;
                }
                true_sign = !true_sign;
            }
            for (size_t i = start + 1; i < M; ++i) {
                Field coef = Gaus.Body[i][j];
                coef /= Gaus.Body[start][j];
                for (size_t jj = j; jj < N; ++jj) {
                    Gaus.Body[i][jj] -= Gaus.Body[start][jj] * coef;
                }
            }
            ++start;
        }
    }
    return { Gaus, true_sign };
}

template <size_t M, size_t N, typename Field>
template<size_t M1, size_t N1>
bool Matrix<M, N, Field>::operator==(const Matrix<M1, N1, Field>& another) const {
    if (M != M1)
        return false;
    if (N != N1)
        return false;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            if (Body[i][j] != another.Body[i][j])
                return false;
        }
    }
    return true;
}

template <size_t M, size_t N, typename Field>
template<size_t M1, size_t N1>
bool Matrix<M, N, Field>::operator!=(const Matrix<M1, N1, Field>& another) const {
    return !(*this == another);
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator+=(const Matrix<M, N, Field>& another) {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Body[i][j] += another.Body[i][j];
        }
    }
    return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator-=(const Matrix<M, N, Field>& another) {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Body[i][j] -= another.Body[i][j];
        }
    }
    return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator*=(Field another) {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Body[i][j] *= another;
        }
    }
    return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(Field another, Matrix<M, N, Field> mat) {
    Matrix<M, N, Field> ans = mat;
    ans *= another;
    return ans;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> mat, Field another) {
    Matrix<M, N, Field> ans = mat;
    ans *= another;
    return ans;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator+(const Matrix<M, N, Field>& first, const Matrix<M, N, Field>& second) {
    Matrix<M, N, Field> answer = first;
    answer += second;
    return answer;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator-(const Matrix<M, N, Field>& first, const Matrix<M, N, Field>& second) {
    Matrix<M, N, Field> answer = first;
    answer -= second;
    return answer;
}

template <size_t M, size_t N, size_t K, typename Field>
Matrix<M, K, Field> operator*(const Matrix<M, N, Field>& first, const Matrix<N, K, Field>& second) {
    bool z = true;
    Matrix<M, K, Field> ans(z);
    for (size_t j = 0; j < M; ++j) {
        for (size_t jj = 0; jj < N; ++jj) {
            for (size_t i = 0; i < K; ++i) {
                ans[j][i] += first[j][jj] * second[jj][i];
            }
        }
    }
    return ans;
}

template <size_t M, size_t N, typename Field>
Matrix<N, M, Field> Matrix<M, N, Field>::transposed() const {
    Matrix<N, M, Field> ans;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            ans[j][i] = Body[i][j];
        }
    }
    return ans;
}

template <size_t M, size_t N, typename Field>
std::vector<Field> Matrix<M, N, Field>::getRow(size_t index) const {
    std::vector<Field> ans;
    ans.resize(N);
    for (size_t j = 0; j < N; ++j) {
        ans[j] = Body[index][j];
    }
    return ans;
}

template <size_t M, size_t N, typename Field>
std::vector<Field> Matrix<M, N, Field>::getColumn(size_t index) const {
    std::vector<Field> ans;
    ans.resize(M);
    for (size_t j = 0; j < M; ++j) {
        ans[j] = Body[j][index];
    }
    return ans;
}

template <size_t M, size_t N, typename Field>
size_t Matrix<M, N, Field>::rank() const {
    Matrix<M, N, Field> Gaus = gaus().first;
    size_t ans = M;
    for (size_t i = 0; i < M; ++i) {
        size_t j = 0;
        for (; j < N; ++j) {
            if (Gaus.Body[M - i - 1][j] != 0) {
                break;
            }
        }
        if (j != N)
            break;
        --ans;
    }
    return ans;
}

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::det() {
    static_assert(M == N);
    std::pair<Matrix<M, M, Field>, bool> pair = gaus();
    Field ans = 1;
    Field change(-1);
    if (!pair.second)
        ans *= change;
    for (size_t i = 0; i < M; ++i) {
        ans *= pair.first.Body[i][i];
    }
    return ans;
}

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::trace() {
    static_assert(M == N);
    Field ans = 0;
    for (size_t i = 0; i < M; ++i) {
        ans += Body[i][i];
    }
    return ans;
}

template <size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::invert() {
    static_assert(M == N);
    Matrix<M, N * 2, Field> Gaus;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Gaus[i][j] = Body[i][j];
        }
    }
    for (size_t j = 0; j < M; ++j) {
        Gaus[j][N + j] = 1;
    }
    for (size_t j = 0; j < N; ++j) {
        size_t begin = j;
        for (; begin < M; ++begin) {
            if (Gaus[begin][j] != 0)
                break;
        }
        if (begin != j) {
            for (size_t i = j; i < 2 * N; ++i) {
                Field swap = Gaus[j][i];
                Gaus[j][i] = Gaus[begin][i];
                Gaus[begin][i] = swap;
            }
        }
        for (size_t jj = j + 1; jj < 2 * N; ++jj) {
            Gaus[j][jj] /= Gaus[j][j];
        }
        Gaus[j][j] = 1;
        for (size_t i = j + 1; i < M; ++i) {
            for (size_t jj = j + 1; jj < 2 * N; ++jj) {
                Gaus[i][jj] -= Gaus[j][jj] * Gaus[i][j];
            }
            Gaus[i][j] = 0;
        }
        for (size_t i = 0; i < j; ++i) {
            for (size_t jj = j + 1; jj < 2 * N; ++jj) {
                Gaus[i][jj] -= Gaus[j][jj] * Gaus[i][j];
            }
            Gaus[i][j] = 0;
        }
    }
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Body[i][j] = Gaus[i][N + j];
        }
    }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::inverted() const {
    static_assert(M == N);
    Matrix<M, N, Field> ans = *this;
    ans.invert();
    return ans;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(const Matrix<M, N, Field>& second) {
    static_assert(M == N);
    bool z = true;
    Matrix<M, N, Field> ans(z);
    for (size_t j = 0; j < M; ++j) {
        for (size_t jj = 0; jj < M; ++jj) {
            for (size_t i = 0; i < M; ++i) {
                ans.Body[j][i] += Body[j][jj] * second.Body[jj][i];
            }
        }
    }
    *this = ans;
    return *this;
}

template <size_t M, size_t N, typename Field>
class String {
private:
    size_t first_index;
    Matrix<M, N, Field>* mat = nullptr;
public:
    String(size_t, Matrix<M, N, Field>*);
    ~String() = default;

    Field& operator[](size_t index) const;
};

template <size_t M, size_t N, typename Field>
class CString {
private:
    size_t first_index;
    const Matrix<M, N, Field>* mat = nullptr;
public:
    CString(size_t, const Matrix<M, N, Field>*);
    ~CString() = default;

    const Field operator[](size_t index) const;
};

template <size_t M, size_t N, typename Field>
String<M, N, Field>::String(size_t index, Matrix<M, N, Field>* base) {
    first_index = index;
    mat = base;
}

template <size_t M, size_t N, typename Field>
CString<M, N, Field>::CString(size_t index, const Matrix<M, N, Field>* base) {
    first_index = index;
    mat = base;
}

template <size_t M, size_t N, typename Field>
Field& String<M, N, Field>::operator[](size_t index) const {
    Field& ans = mat->Body[first_index][index];
    return ans;
}

template <size_t M, size_t N, typename Field>
const Field CString<M, N, Field>::operator[](size_t index) const {
    const Field& ans = mat->Body[first_index][index];
    return ans;
}

template <size_t M, size_t N, typename Field>
String<M, N, Field> Matrix<M, N, Field>::operator[](size_t index) {
    String<M, N, Field> ans(index, this);
    return ans;
}

template <size_t M, size_t N, typename Field>
const CString<M, N, Field> Matrix<M, N, Field>::operator[](size_t index) const {
    const CString<M, N, Field> ans(index, this);
    return ans;
}

template <size_t N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;
