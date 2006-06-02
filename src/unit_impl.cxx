#include "StdAfx.h"

extern "C" {
#define EXTERNAL extern
#include "srcinput/hstinpt.h"
#undef EXTERNAL
}

#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unit::unit(void)
: input(0)
, si(0)
, input_to_si(1.0)
, input_to_user(1.0)
, defined(FALSE)
{
}

unit::unit(const char* si)
: input(0)
, si(0)
, input_to_si(1.0)
, input_to_user(1.0)
, defined(FALSE)
{
	this->si = new char[::strlen(si) + 1];
	::strcpy(this->si, si);
}

unit::~unit(void)
{
	delete si;
	delete input;
}

unit::unit(const unit& src)
: input(0)
, si(0)
, input_to_si(src.input_to_si)
, input_to_user(src.input_to_user)
, defined(src.defined)
{
	if (src.input) {
		this->input = new char[::strlen(src.input) + 1];
		::strcpy(this->input, src.input);
	}
	if (src.si) {
		this->si = new char[::strlen(src.si) + 1];
		::strcpy(this->si, src.si);
	}
}

unit& unit::operator=(const unit& rhs)
{
	if (this != &rhs) {
		delete this->input;
		this->input = 0;
		if (rhs.input) {
			this->input = new char[::strlen(rhs.input) + 1];
			::strcpy(this->input, rhs.input);
		}

		delete this->si;
		this->si = 0;
		if (rhs.si) {
			this->si = new char[::strlen(rhs.si) + 1];
			::strcpy(this->si, rhs.si);
		}

		this->input_to_si   = rhs.input_to_si;
		this->input_to_user = rhs.input_to_user;
		this->defined       = rhs.defined;
	}
	return *this;
}

int unit::set_input(const char* input)
{
	if (!input) return ERROR;

	if (this->defined == TRUE) {
		assert(this->input);
		delete this->input;
	}
	this->defined = TRUE;
	this->input = new char[::strlen(input) + 1];
	::strcpy(this->input, input);

	assert(this->si);
	int n = ::units_conversion(this->input, this->si, &this->input_to_si, FALSE);
	if (n != OK) {
		delete this->input;
		this->input = 0;
		this->defined = FALSE;
	}
	else {
		if (this->input_to_si == 1.0) {
			delete this->input;
			this->input = 0;
			this->defined = FALSE;
		}
	}
	return n;
}

const char* unit::c_str(void)const
{
	if (this->defined == TRUE) {
		assert(this->input);
		return this->input;
	}
	else {
		assert(this->si);
		return this->si;
	}
}