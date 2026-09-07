#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>

struct Point {
    double x;
    double y;

    Point() = default;

    Point(double x_, double y_) {
        x = x_;
        y = y_;
    }

    Point& operator=(const Point& point) {
        x = point.x;
        y = point.y;
        return *this;
    }

    bool operator==(const Point& point) const {
        if (std::abs(x - point.x) < 0.000001 && std::abs(y - point.y) < 0.000001)
            return true;
        return false;
    }

    bool operator!=(const Point& point) const {
        return !(*this == point);
    }

    Point operator+(const Point& another) const {
        Point point(x + another.x, y + another.y);
        return point;
    }

    Point operator-(const Point& another) const {
        Point point(x - another.x, y - another.y);
        return point;
    }

    Point operator*(double another) const {
        Point point(x * another, y * another);
        return point;
    }

    Point operator/(double another) const {
        Point point(x / another, y / another);
        return point;
    }

    void rotate(Point center, double angle);
    void reflex(Point center);
    void reflex(class Line axis);
    void scale(Point center, double coefficient);

    Point(class Line first, class Line second);
};

void Point::rotate(Point center, double angle) {
    double pi = 3.141592653589793;
    double x2 = x;
    double y2 = y;
    double x1 = center.x;
    double y1 = center.y;
    x = x1 + (x2 - x1) * cos(angle * pi / 180) - (y2 - y1) * sin(angle * pi / 180);
    y = y1 + (x2 - x1) * sin(angle * pi / 180) + (y2 - y1) * cos(angle * pi / 180);
}

void Point::reflex(Point center) {
    *this = center + center - *this;
}

void Point::scale(Point center, double coefficient) {
    *this = center + ((*this - center) * coefficient);
}

double Length(const struct Point& first, const struct Point& second) {
    return sqrt((first.x - second.x) * (first.x - second.x) + (first.y - second.y) * (first.y - second.y));
}

class Line {
private:
    double a;
    double b;
    double c;

    friend Point;
public:

    Line(double k_, double c_);
    Line(const struct Point& point, double k_);
    Line(double k_, const struct Point& point);
    Line(const struct Point& first, const struct Point& second);

    bool operator==(const Line& line) const;

    bool operator!=(const Line& line) const;

    Line(const Line& another, const struct Point& point);
};

Line::Line(double k_, double c_) {
    a = k_;
    b = -1;
    c = c_;
}

Line::Line(const struct Point& point, double k_) {
    a = k_;
    b = -1;
    c = point.y - (point.x * k_);
}

Line::Line(double k_, const struct Point& point) {
    a = k_;
    b = -1;
    c = point.y - (point.x * k_);
}

Line::Line(const struct Point& first, const struct Point& second) {
    if (first.x == second.x) {
        b = 0;
        a = 1;
        c = -first.x;
        return;
    }
    if (first.y == second.y) {
        b = 1;
        a = 0;
        c = -first.y;
        return;
    }
    b = -1;
    c = (second.y * first.x - second.x * first.y) / (first.x - second.x);
    if (first.x != 0) {
        a = (first.y - c) / first.x;
        return;
    }
    a = (second.y - c) / second.x;
}

Line::Line(const Line& another, const struct Point& point) {
    a = -another.b;
    b = another.a;
    c = -(a * point.x + b * point.y);
}

bool Line::operator==(const Line& line) const {
    if (std::abs(a * line.b - b * line.a) < 0.000001 && std::abs(a * line.c - c * line.a) < 0.000001 && std::abs(c * line.b - b * line.c) < 0.000001)
        return true;
    return false;
}

bool Line::operator!=(const Line& line) const {
    return !(*this == line);
}

Point::Point(class Line first, class Line second) {
    double det = first.a * second.b - first.b * second.a;
    y = (first.c * second.a - first.a * second.c) / det;
    x = (first.b * second.c - first.c * second.b) / det;
}

void Point::reflex(class Line axis) {
    Line line(axis, *this);
    Point center(axis, line);
    reflex(center);
}

double Length(const class Line& line, const struct Point& first) {
    return Length(first, Point(line, Line(line, first)));
}

class Shape {
protected:
    const double pi = 3.141592653589793;
public:
    int shape_type = 0;

    Shape() = default;
    virtual ~Shape() = 0;

    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool operator==(const Shape& another) const = 0;
    virtual bool operator!=(const Shape& another) const = 0;
    virtual bool isCongruentTo(const Shape& another) const = 0;
    virtual bool isSimilarTo(const Shape& another)  const = 0;
    virtual bool containsPoint(Point point)  const = 0;

    virtual void rotate(Point center, double angle) = 0;
    virtual void reflex(Point center) = 0;
    virtual void reflex(Line axis) = 0;
    virtual void scale(Point center, double coefficient) = 0;
};

Shape::~Shape() {}

class Polygon : public Shape {
protected:
    std::vector <struct Point> Vertex;
public:
    Polygon();
    Polygon(std::vector <Point> Vertex_);
    Polygon(std::initializer_list<Point> points);

    Polygon& operator=(const Polygon& polygon);

    double perimeter() const;
    double area() const;
    bool operator==(const Shape& another) const;
    bool operator!=(const Shape& another) const;
    bool isCongruentTo(const Shape& another) const;
    bool isSimilarTo(const Shape& another)  const;
    bool containsPoint(Point point)  const;

    void rotate(Point center, double angle);
    void reflex(Point center);
    void reflex(Line axis);
    void scale(Point center, double coefficient);

    int verticesCount();
    std::vector<struct Point> getVertices();
    bool isConvex();
};

Polygon::Polygon(std::vector <struct Point> Vertex_) : Vertex(Vertex_) {
    shape_type = 2;
}

Polygon::Polygon(std::initializer_list<Point> points) : Vertex(points) {
    shape_type = 2;
}

Polygon::Polygon() {
    shape_type = 2;
}

Polygon& Polygon::operator=(const Polygon& polygon) {
    Vertex = polygon.Vertex;
    return *this;
}

int Polygon::verticesCount() {
    return Vertex.size();
}

std::vector<struct Point> Polygon::getVertices() {
    return Vertex;
}

double Polygon::perimeter() const {
    double ans = Length(Vertex[0], Vertex[Vertex.size() - 1]);
    for (size_t i = 1; i < Vertex.size(); ++i)
        ans += Length(Vertex[i], Vertex[i - 1]);
    return ans;
}

double Polygon::area() const {
    double ans = (Vertex[0].x - Vertex[Vertex.size() - 1].x) * (Vertex[0].y + Vertex[Vertex.size() - 1].y);
    for (size_t i = 1; i < Vertex.size(); ++i) {
        ans += (Vertex[i].x - Vertex[i - 1].x) * (Vertex[i].y + Vertex[i - 1].y);
    }
    return std::abs(ans / 2);
}

bool Polygon::operator==(const Shape& another) const {
    if (dynamic_cast<const Polygon*>(&another) == nullptr) {
        return false;
    }
    const Polygon& pol = dynamic_cast<const Polygon&> (another);
    if (Vertex.size() != pol.Vertex.size()) {
        return false;
    }
    size_t i = 0;
    for (; i < Vertex.size(); ++i) {
        if (Vertex[i] == pol.Vertex[0])
            break;
    }
    if (i == Vertex.size()) {
        return false;
    }
    size_t count = 0;
    for (size_t j = 0; j < Vertex.size(); ++j) {
        if (Vertex[(i + j) % Vertex.size()] == pol.Vertex[j])
            ++count;
    }
    if (count == Vertex.size()) {
        return true;
    }
    count = 0;
    for (size_t j = 0; j < Vertex.size(); ++j) {
        if (Vertex[(Vertex.size() + i - j) % Vertex.size()] == pol.Vertex[j])
            ++count;
    }
    if (count == Vertex.size())
        return true;

    return false;

}

bool Polygon::operator!=(const Shape& another) const {
    return !(*this == another);
}

bool Polygon::isCongruentTo(const Shape& another) const {
    if (!isSimilarTo(another))
        return false;
    const Polygon& pol = dynamic_cast<const Polygon&> (another);
    if (std::abs(pol.perimeter() - perimeter()) < 0.000001) {
        return true;
    }
    return false;
}

bool Polygon::isSimilarTo(const Shape& another)  const {
    if (dynamic_cast<const Polygon*>(&another) == nullptr) {
        return false;
    }
    if (shape_type != another.shape_type) {
        return false;
    }
    const Polygon& pol = dynamic_cast<const Polygon&> (another);
    if (Vertex.size() != pol.Vertex.size()) {
        return false;
    }
    size_t count = 0;
    size_t n = Vertex.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            size_t current = (i + j);
            double first = Length(Vertex[current % n], Vertex[(current + 1) % n]);
            double second = Length(Vertex[(current + 1) % n], Vertex[(current + 2) % n]);
            double pfirst = Length(pol.Vertex[j % n], pol.Vertex[(j + 1) % n]);
            double psecond = Length(pol.Vertex[(j + 1) % n], pol.Vertex[(j + 2) % n]);
            double height = Length(Line(Vertex[current % n], Vertex[(current + 1) % n]), Vertex[(current + 2) % n]);
            double pheight = Length(Line(pol.Vertex[j % n], pol.Vertex[(j + 1) % n]), pol.Vertex[(j + 2) % n]);
            if (std::abs(first * psecond - pfirst * second) < 0.000001 && std::abs(first * pheight - height * pfirst) < 0.000001)
                ++count;
        }
        if (count == n) {
            return true;
        }
        count = 0;
    }
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            size_t current = (n + i - j);
            double second = Length(Vertex[current % n], Vertex[(current + 1) % n]);
            double first = Length(Vertex[(current + 1) % n], Vertex[(current + 2) % n]);
            double pfirst = Length(pol.Vertex[j % n], pol.Vertex[(j + 1) % n]);
            double psecond = Length(pol.Vertex[(j + 1) % n], pol.Vertex[(j + 2) % n]);
            double height = Length(Line(Vertex[(current + 2) % n], Vertex[(current + 1) % n]), Vertex[(current) % n]);
            double pheight = Length(Line(pol.Vertex[j % n], pol.Vertex[(j + 1) % n]), pol.Vertex[(j + 2) % n]);
            if (std::abs(first * psecond - pfirst * second) < 0.000001 && std::abs(first * pheight - height * pfirst) < 0.000001)
                ++count;
        }
        if (count == n) {
            return true;
        }
        count = 0;
    }
    return false;
}

bool Polygon::containsPoint(Point point)  const {
    bool count = false;
    size_t n = Vertex.size();
    for (size_t i = 0; i < Vertex.size(); ++i) {
        bool between = (Vertex[i].y <= point.y && Vertex[(i + n - 1) % n].y >= point.y) || (Vertex[(i + n - 1) % n].y <= point.y && Vertex[i].y >= point.y);
        bool right = Point(Line(Vertex[i], Vertex[(i + n - 1) % n]), Line(0, point)).x < point.x;
        if (between && right)
            count = !count;
    }
    return count;
}

void Polygon::rotate(Point center, double angle) {
    for (size_t i = 0; i < Vertex.size(); ++i)
        Vertex[i].rotate(center, angle);
}

void Polygon::reflex(Point center) {
    for (size_t i = 0; i < Vertex.size(); ++i)
        Vertex[i].reflex(center);
}

void Polygon::reflex(Line axis) {
    for (size_t i = 0; i < Vertex.size(); ++i)
        Vertex[i].reflex(axis);
}

void Polygon::scale(Point center, double coefficient) {
    for (size_t i = 0; i < Vertex.size(); ++i)
        Vertex[i].scale(center, coefficient);
}

bool Polygon::isConvex() {
    size_t n = Vertex.size();
    Point first = Vertex[0] - Vertex[n - 1];
    Point second = Vertex[1] - Vertex[0];
    bool is_positive = true;
    if ((first.x * second.y - first.y * second.x) < 0)
        is_positive = false;
    for (size_t i = 2; i < n; ++i) {
        first = Vertex[i - 1] - Vertex[i - 2];
        second = Vertex[i] - Vertex[i - 1];
        double number = (first.x * second.y - first.y * second.x);
        if ((is_positive && number < 0) || (!is_positive && number > 0))
            return false;
    }
    first = Vertex[n - 1] - Vertex[n - 2];
    second = Vertex[0] - Vertex[n - 1];
    double number = (first.x * second.y - first.y * second.x);
    if ((is_positive && number < 0) || (!is_positive && number > 0))
        return false;
    return true;
}

class Ellipse : public Shape {
protected:
    Point focus1;
    Point focus2;
    double rad;
public:
    Ellipse();
    Ellipse(Point focus1_, Point focus2_, double radius_);

    Ellipse& operator=(const Ellipse& ellipse);

    double perimeter() const;
    double area() const;
    bool operator==(const Shape& another) const;
    bool operator!=(const Shape& another) const;
    bool isCongruentTo(const Shape& another) const;
    bool isSimilarTo(const Shape& another)  const;
    bool containsPoint(Point point)  const;

    void rotate(Point center, double angle);
    void reflex(Point center);
    void reflex(Line axis);
    void scale(Point center, double coefficient);

    std::pair<Point, Point> focuses() const;
    double eccentricity() const;
    Point center() const;

    double a() const;
    double b() const;
};

Ellipse::Ellipse() {
    shape_type = 1;
}

Ellipse::Ellipse(Point focus1_, Point focus2_, double radius_) : focus1(focus1_), focus2(focus2_), rad(radius_) {
    shape_type = 1;
}

Ellipse& Ellipse::operator=(const Ellipse& ellipse) {
    focus1 = ellipse.focus1;
    focus2 = ellipse.focus2;
    rad = ellipse.rad;
    return *this;
}

std::pair<Point, Point> Ellipse::focuses() const {
    return { focus1, focus2 };
}

double Ellipse::a() const {
    return rad / 2;
}

double Ellipse::b() const {
    return sqrt(a() * a() - Length(focus1, center()) * Length(focus1, center()));
}

double Ellipse::perimeter() const {
    return 4 * a() * std::comp_ellint_2(Length(focus1, center()) / a());
}

double Ellipse::area() const {
    return pi * a() * b();
}

bool Ellipse::operator==(const Shape& another) const {
    if (dynamic_cast<const Polygon*>(&another) == nullptr)
        return false;
    const Ellipse& el = dynamic_cast<const Ellipse&> (another);
    if (Length(focus1, el.focuses().first) < 0.000001 && Length(focus2, el.focuses().second) < 0.000001 && std::abs(a() - el.a()) < 0.000001) {
        return true;
    }
    if (Length(focus2, el.focuses().first) < 0.000001 && Length(focus1, el.focuses().second) < 0.000001 && std::abs(a() - el.a()) < 0.000001) {
        return true;
    }
    return false;
}

bool Ellipse::operator!=(const Shape& another) const {
    return !(*this == another);
}

bool Ellipse::isCongruentTo(const Shape& another) const {
    if (dynamic_cast<const Polygon*>(&another) == nullptr)
        return false;
    const Ellipse& el = dynamic_cast<const Ellipse&> (another);
    if (std::abs(a() - el.a()) < 0.000001 && std::abs(b() - el.b()) < 0.000001) {
        return true;
    }
    if (std::abs(a() - el.b()) < 0.000001 && std::abs(b() - el.a()) < 0.000001) {
        return true;
    }
    return false;
}

bool Ellipse::isSimilarTo(const Shape& another)  const {
    if (dynamic_cast<const Polygon*>(&another) == nullptr)
        return false;
    const Ellipse& el = dynamic_cast<const Ellipse&> (another);
    if (std::abs((a() * el.a()) - (b() * el.b())) < 0.000001) {
        return true;
    }
    if (std::abs((a() * el.b()) - (b() * el.a())) < 0.000001) {
        return true;
    }
    return false;
}

bool Ellipse::containsPoint(Point point)  const {
    if ((Length(point, focus1) + Length(point, focus2)) <= rad)
        return true;
    return false;
}

double Ellipse::eccentricity() const {
    return Length(focus1, focus2) / rad;
}

Point Ellipse::center() const {
    return (focus1 + focus2) / 2;
}


void Ellipse::rotate(Point center, double angle) {
    focus1.rotate(center, angle);
    focus2.rotate(center, angle);
}

void Ellipse::reflex(Point center) {
    focus1.reflex(center);
    focus2.reflex(center);
}

void Ellipse::reflex(Line axis) {
    focus1.reflex(axis);
    focus2.reflex(axis);
}

void Ellipse::scale(Point center, double coefficient) {
    focus1.scale(center, coefficient);
    focus2.scale(center, coefficient);
    rad *= coefficient;
}

class Circle : public Ellipse {
public:
    Circle() = default;
    Circle(Point focus_, double radius_);
    Circle(Point first, Point second, Point third);

    double radius() const;
};

Circle::Circle(Point focus_, double radius_) {
    focus1 = focus_;
    focus2 = focus_;
    rad = 2 * radius_;
}

Circle::Circle(Point first, Point second, Point third) {
    Line f_s(first, second);
    Line s_t(third, second);
    Line f(f_s, (first + second) / 2);
    Line s(s_t, (third + second) / 2);
    Point focus_(f, s);
    double radius_ = Length(focus_, first);
    focus1 = focus_;
    focus2 = focus_;
    rad = 2 * radius_;
}

double Circle::radius() const {
    return rad / 2;
}

class Rectangle : public Polygon {
protected:

public:
    Rectangle() = default;
    Rectangle(Point first, Point second, double coefficient);

    Point center();
    std::pair<Line, Line> diagonals();
};

Rectangle::Rectangle(Point first, Point second, double coefficient) {
    if (coefficient > 1)
        coefficient = 1 / coefficient;
    Vertex.resize(4);
    Vertex[0] = first;
    Vertex[2] = second;
    double gomotet = 1 / (1 + coefficient * coefficient);
    Point current_second = second;
    Point current_first = first;
    current_second.scale(first, gomotet);
    current_first.rotate(current_second, 270);
    current_first.scale(current_second, 1 / coefficient);
    current_second = (Vertex[0] + Vertex[2]) - current_first;
    Vertex[1] = current_first;
    Vertex[3] = current_second;
}

Point Rectangle::center() {
    return (Vertex[0] + Vertex[2]) / 2;
}

std::pair<Line, Line> Rectangle::diagonals() {
    Line first(Vertex[0], Vertex[2]);
    Line second(Vertex[1], Vertex[3]);
    return { first, second };
}

class Square : public Rectangle {
protected:

public:
    Square() = default;
    Square(Point first, Point second);

    class Circle circumscribedCircle();
    class Circle inscribedCircle();
};

Square::Square(Point first, Point second) {
    Vertex.resize(4);
    Vertex[0] = first;
    Vertex[2] = second;
    double gomotet = (double)1 / (double)2;
    Point current_second = second;
    Point current_first = first;
    current_second.scale(first, gomotet);
    current_first.rotate(current_second, 270);
    current_second = (Vertex[0] + Vertex[2]) - current_first;
    Vertex[1] = current_first;
    Vertex[3] = current_second;
}

class Circle Square::circumscribedCircle() {
    Circle circle(Vertex[0], Vertex[1], Vertex[2]);
    return circle;
}

class Circle Square::inscribedCircle() {
    Circle circle((Vertex[0] + Vertex[1]) / 2, (Vertex[1] + Vertex[2]) / 2, (Vertex[2] + Vertex[3]) / 2);
    return circle;
}

class Triangle : public Polygon {
protected:

public:
    Triangle() = default;
    Triangle(Point first, Point second, Point third);

    class Circle circumscribedCircle();
    class Circle inscribedCircle();
    Point centroid();
    Point orthocenter();
    Line EulerLine();
    class Circle ninePointsCircle();
};

Triangle::Triangle(Point first, Point second, Point third) {
    Vertex.resize(3);
    Vertex[0] = first;
    Vertex[1] = second;
    Vertex[2] = third;
}

class Circle Triangle::circumscribedCircle() {
    Circle circle(Vertex[0], Vertex[1], Vertex[2]);
    return circle;
}

class Circle Triangle::inscribedCircle() {
    Line f_s(Vertex[0], Vertex[1]);
    Line s_t(Vertex[2], Vertex[1]);
    double zero_one = Length(Vertex[0], Vertex[1]);
    double zero_two = Length(Vertex[0], Vertex[2]);
    double one_two = Length(Vertex[2], Vertex[1]);
    Line f(Vertex[2], ((Vertex[0] * one_two) + (Vertex[1] * zero_two)) / (one_two + zero_two));
    Line s(Vertex[0], ((Vertex[1] * zero_two) + (Vertex[2] * zero_one)) / (zero_one + zero_two));
    Point focus_(f, s);
    double radius_ = Length(f_s, focus_);
    Circle circle(focus_, radius_);
    return circle;
}

Point Triangle::centroid() {
    return Point(Line((Vertex[0] + Vertex[1]) / (double) 2, Vertex[2]), Line((Vertex[1] + Vertex[2]) / (double) 2, Vertex[0]));
}

Point Triangle::orthocenter() {
    return Point(Line(Line(Vertex[0], Vertex[1]), Vertex[2]), Line(Line(Vertex[0], Vertex[2]), Vertex[1]));
}

Line Triangle::EulerLine() {
    return Line(centroid(), orthocenter());
}

class Circle Triangle::ninePointsCircle() {
    return Circle((Vertex[0] + Vertex[1]) / 2, (Vertex[1] + Vertex[2]) / 2, (Vertex[2] + Vertex[0]) / 2);
}
