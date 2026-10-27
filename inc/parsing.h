/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:25:27 by coco              #+#    #+#             */
/*   Updated: 2026/10/27 12:15:48 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include "ft_script.h"

# define FLAG_LIST				{"-a", "--append", "-c", "--command", "-E", "--echo", "-e", "--return", \
								"-f", "--flush", "-B", "--log-io", "-I", "--log-in", "-O", "--log-out", \
								"-T", "--log-timing", "-m", "--logging-format", "-o", "--output-limit", \
								"-q", "--quiet", "-t", "--timing", "-h", "--help", \
								"-V", "--version", "--force"}
# define FLAG_COUNT				31

# define ECHO_MODE_LIST			{"auto", "always", "never"}
# define ECHO_MODE_COUNT		3

# define LOGGING_FORMAT_LIST	{"classic", "advanced"}
# define LOGGING_FORMAT_COUNT	2

/*				parser error				*/
# define UNKNOWN_FLAG				"Unknown flag: "
# define UNKNOWN_PARAM				"invalid option: "
# define UNSSUPORTED_ECHO			"unssuported echo mode: "
# define UNSSUPORTED_FORMAT			"unsupported logging format: "
# define MISSING_ARG				"option requires an argument "
# define UNEXPECTED_ARG				"unexpected number of arguments"
# define FAILED_TO_PARSE_LIMIT		"failed to parse output limit size: "
# define HELP_INFO					"Try 'ft_script --help' for more information.\n"
# define HELP_FLAG					"Usage:\n ft_script [options] [file]\n\nMake a typescript of a terminal session.\n\n\
Options:\n -I, --log-in <file>           log stdin to file\n -O, --log-out <file>          log stdout to file (default)\n\
 -B, --log-io <file>           log stdin and stdout to file\n\n -T, --log-timing <file>       log timing information to file\n\
 -t[<file>], --timing[=<file>] deprecated alias to -T (default file is stderr)\n -m, --logging-format <name>   force to 'classic' or 'advanced' format\n\n\
 -a, --append                  append to the log file\n -c, --command <command>       run command rather than interactive shell\n\
 -e, --return                  return exit code of the child process\n -f, --flush                   run flush after each write\n\
     --force                   use output file even when it is a link\n -E, --echo <when>             echo input in session (auto, always or never)\n\
 -o, --output-limit <size>     terminate if output files exceed size\n -q, --quiet                   be quiet\n\n\
 -h, --help                    display this help\n -V, --version                 display version\n"
# define VERSION_FLAG "ft_script from cpapot\n"
 
bool	is_flags(char *str);
bool	is_in_string(char a, char *str);
bool	is_all_numbers(char *str);
int		parsing_error(char *errorType, char *errorInfo, bool printHelp, int retValue);

enum	e_parsing_flags {
	UNKNOWN_F = 0,
	APPEND_F,
	COMMAND_F,
	ECHO_F,
	RETURN_F,
	FLUSH_F,
	LOG_IO_F,
	LOG_IN_F,
	LOG_OUT_F,
	LOG_TIMING_F,
	LOGGING_FORMAT_F,
	OUTPUT_LIMIT_F,
	QUIET_F,
	TIMING_F,
	HELP_F,
	VERSION_F,
	FORCE_F
};

enum	e_parsing_return {
	SUCCES = 0,
	FAILED = 1
};


#endif
