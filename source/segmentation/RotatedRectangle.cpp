#include "RotatedRectangle.hpp"

RotatedRectangle::RotatedRectangle()
	: p1(cv::Point2f(0.0f, 0.0f))
	, p2(cv::Point2f(0.0f, 0.0f))
	, p3(cv::Point2f(0.0f, 0.0f))
	, p4(cv::Point2f(0.0f, 0.0f))
{
}

RotatedRectangle::RotatedRectangle(const Json & json)
	: p1(json["p1.x"], json["p1.y"])
	, p2(json["p2.x"], json["p2.y"])
	, p3(json["p3.x"], json["p3.y"])
	, p4(json["p4.x"], json["p4.y"])
{
}

RotatedRectangle::RotatedRectangle(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f p4)
	: p1(p1)
	, p2(p2)
	, p3(p3)
	, p4(p4)
{
}

const cv::Point2f& RotatedRectangle::P1() const
{
	return p1;
}

const cv::Point2f& RotatedRectangle::P2() const
{
	return p2;
}

const cv::Point2f& RotatedRectangle::P3() const
{
	return p3;
}

const cv::Point2f& RotatedRectangle::P4() const
{
	return p4;
}

RotatedRectangle::operator Json()
{
	Json j;
	j["p1.x"] = p1.x;
	j["p1.y"] = p1.y;

	j["p2.x"] = p2.x;
	j["p2.y"] = p2.y;

	j["p3.x"] = p3.x;
	j["p3.y"] = p3.y;

	j["p4.x"] = p4.x;
	j["p4.y"] = p4.y;

	return j;
}

RotatedRectangle::RotatedRectangle(const RotatedRectangle& rotatedRectangle)
	: p1(rotatedRectangle.p1)
	, p2(rotatedRectangle.p2)
	, p3(rotatedRectangle.p3)
	, p4(rotatedRectangle.p4)
{
}

RotatedRectangle& RotatedRectangle::operator = (const RotatedRectangle& rotatedRectangle)
{
	p1 = rotatedRectangle.p1;
	p2 = rotatedRectangle.p2;
	p3 = rotatedRectangle.p3;
	p4 = rotatedRectangle.p4;

	return *this;
}

RotatedRectangle& RotatedRectangle::operator * (const size_t& value)
{
	p1.x *= value;
	p1.y *= value;

	p2.x *= value;
	p2.y *= value;

	p3.x *= value;
	p3.y *= value;

	p4.x *= value;
	p4.y *= value;

	return *this;
}