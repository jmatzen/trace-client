#include <trace/trace.h>
#include <string>

void parse_inline_token(char* s, size_t slen, const char* tok, const ayxia_trace_arg* args, size_t nargs)
{

}

void parse_index_token(char* s, size_t slen, const char* tok, const ayxia_trace_arg* args, size_t nargs)
{

}


extern"C" void ayxia_tc_format(
  char* s,
  size_t slen,
  const char* format, 
  const ayxia_trace_arg* args, 
  size_t nargs)
{

  auto tok_begin = strpbrk(format, "%{");
  if (*tok_begin == '%')
    parse_inline_token(s, slen, tok_begin, args, nargs);
  else if (*tok_begin == '{')
    parse_index_token(s, slen, tok_begin, args, nargs);

}


