// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef _H_EXCEPT_H_
#define _H_EXCEPT_H_

#include <ray/err.h>

_NAME_BEGIN

class EXPORT exception : public std::exception
{
public:
	exception() noexcept;
	virtual ~exception() noexcept;

	virtual const char* what() const noexcept = 0;
	virtual const error_code& code() const noexcept = 0;

private:
	exception(const exception&) noexcept = delete;
	exception& operator=(const exception&) noexcept = delete;
};

class EXPORT failure : public exception
{
public:
	failure(const error_code& code) noexcept;
	failure(const error_code::int_type& code) noexcept;
	failure(const char* msg, const error_code& code = make_error_code(error_code::none)) noexcept;
	failure(const char* msg, const char* stack, const error_code& code = make_error_code(error_code::none)) noexcept;
	failure(const util::string& msg, const error_code& code = make_error_code(error_code::none)) noexcept;
	failure(const util::string& msg, const util::string& stack, const error_code& code = make_error_code(error_code::none)) noexcept;
	failure(failure&& move) noexcept;
	virtual ~failure() noexcept;

	const char* message() const noexcept;
	const char* stack() const noexcept;
	const char* what() const noexcept;

	const error_code& code() const noexcept;

private:
	void printStack() noexcept;

private:
	failure(const failure&) noexcept = delete;
	failure& operator=(const failure&) noexcept = delete;

private:
	util::string _info;
	util::string _message;
	util::string _stack;
	error_code _code;
};

_NAME_END

#endif
