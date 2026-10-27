#include "parsing.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define TEST_RESET "\033[0m"
#define TEST_RED "\033[1;31m"
#define TEST_GREEN "\033[1;32m"
#define TEST_CYAN "\033[1;36m"
#define TEST_YELLOW "\033[1;33m"

int parsing(int argc, char **argv, t_script_settings *data);

#define FLAG_APPEND (1u << 0)
#define FLAG_COMMAND (1u << 1)
#define FLAG_RETURN (1u << 2)
#define FLAG_FLUSH (1u << 3)
#define FLAG_FORCE (1u << 4)
#define FLAG_LOG_IO (1u << 5)
#define FLAG_LOG_IN (1u << 6)
#define FLAG_LOG_OUT (1u << 7)
#define FLAG_LOG_TIMING (1u << 8)
#define FLAG_QUIET (1u << 9)

static int tests_run;
static int tests_passed;
int parsing_test_fd;

static int same_string(char *actual, const char *expected)
{
	if (actual == NULL || expected == NULL)
		return (actual == expected);
	return (ft_strcmp(actual, expected));
}

static int has_flags(t_script_settings *data, unsigned int expected)
{
	unsigned int actual;

	actual = 0;
	if (data->isAppend)
		actual |= FLAG_APPEND;
	if (data->command != NULL)
		actual |= FLAG_COMMAND;
	if (data->isReturn)
		actual |= FLAG_RETURN;
	if (data->isFlush)
		actual |= FLAG_FLUSH;
	if (data->isForce)
		actual |= FLAG_FORCE;
	if (data->logIo != NULL)
		actual |= FLAG_LOG_IO;
	if (data->logIn != NULL)
		actual |= FLAG_LOG_IN;
	if (data->logOut != NULL)
		actual |= FLAG_LOG_OUT;
	if (data->logTiming != NULL)
		actual |= FLAG_LOG_TIMING;
	if (data->isQuiet)
		actual |= FLAG_QUIET;
	return (actual == expected);
}

static unsigned int get_flags(t_script_settings *data)
{
	unsigned int flags;

	flags = 0;
	if (data->isAppend)
		flags |= FLAG_APPEND;
	if (data->command != NULL)
		flags |= FLAG_COMMAND;
	if (data->isReturn)
		flags |= FLAG_RETURN;
	if (data->isFlush)
		flags |= FLAG_FLUSH;
	if (data->isForce)
		flags |= FLAG_FORCE;
	if (data->logIo != NULL)
		flags |= FLAG_LOG_IO;
	if (data->logIn != NULL)
		flags |= FLAG_LOG_IN;
	if (data->logOut != NULL)
		flags |= FLAG_LOG_OUT;
	if (data->logTiming != NULL)
		flags |= FLAG_LOG_TIMING;
	if (data->isQuiet)
		flags |= FLAG_QUIET;
	return (flags);
}

static void print_input(int argc, char **argv)
{
	int index;

	ft_printf("    argv: [");
	for (index = 0; index < argc; index++)
	{
		if (index != 0)
			ft_printf(", ");
		ft_printf("%s", argv[index]);
	}
	ft_printf("]\n");
}

static void print_diff(int result, int expected_ret, t_script_settings *data,
	const char *target, const char *command, int echo_mode, off_t output_limit,
	unsigned int flags, bool closing)
{
	ft_printf("    diff:\n");
	if (result != expected_ret)
		ft_printf("      return: got %d, expected %d\n", result, expected_ret);
	if (!same_string(data->target, target))
		ft_printf("      target: got %s, expected %s\n", data->target, target);
	if (!same_string(data->command, command))
		ft_printf("      command: got %s, expected %s\n", data->command, command);
	if (data->echoMode != echo_mode)
		ft_printf("      echoMode: got %d, expected %d\n", data->echoMode, echo_mode);
	if (data->outputLimit != output_limit)
		ft_printf("      outputLimit: got %u, expected %u\n",
			(unsigned int)data->outputLimit, (unsigned int)output_limit);
	if (get_flags(data) != flags)
		ft_printf("      flags: got 0x%x, expected 0x%x\n", get_flags(data), flags);
	if (data->isClosing != closing)
		ft_printf("      isClosing: got %s, expected %s\n",
			data->isClosing ? "true" : "false", closing ? "true" : "false");
}

static void check_case(const char *name, int actual, int expected, int argc,
	char **argv, int result, int expected_ret, t_script_settings *data,
	const char *target, const char *command, int echo_mode, off_t output_limit,
	unsigned int flags, bool closing)
{
	tests_run++;
	if (actual == expected)
	{
		tests_passed++;
		ft_printf("%s  PASS%s %s\n", TEST_GREEN, TEST_RESET, name);
	}
	else
	{
		ft_printf("%s  FAIL%s %s\n", TEST_RED, TEST_RESET, name);
		print_input(argc, argv);
		print_diff(result, expected_ret, data, target, command, echo_mode,
			output_limit, flags, closing);
	}
}

static void parse_case(const char *name, int argc, char **argv, int expected_ret,
	const char *target, const char *command, int echo_mode, off_t output_limit,
	unsigned int flags, bool closing)
{
	t_script_settings data;
	int result;
	int valid;

	ft_bzero(&data, sizeof(data));
	result = parsing(argc, argv, &data);
	valid = result == expected_ret;
	valid = valid && same_string(data.target, target);
	valid = valid && same_string(data.command, command);
	valid = valid && data.echoMode == echo_mode;
	valid = valid && data.outputLimit == output_limit;
	valid = valid && has_flags(&data, flags);
	valid = valid && data.isClosing == closing;
	check_case(name, valid, 1, argc, argv, result, expected_ret, &data, target,
		command, echo_mode, output_limit, flags, closing);
}

static void parse_timing_case(const char *name, int argc, char **argv,
	int expected_ret, const char *target, const char *timing)
{
	t_script_settings data;
	int result;
	int valid;

	ft_bzero(&data, sizeof(data));
	result = parsing(argc, argv, &data);
	valid = result == expected_ret;
	valid = valid && same_string(data.target, target);
	valid = valid && same_string(data.timing, timing);
	check_case(name, valid, 1, argc, argv, result, expected_ret, &data,
		target, NULL, ECHO_AUTO, 0, 0, false);
	if (!valid && !same_string(data.timing, timing))
		ft_printf("      timing: got %s, expected %s\n", data.timing, timing);
}

static void check_return_case(const char *name, int result, int expected)
{
	tests_run++;
	if (result == expected)
	{
		tests_passed++;
		ft_printf("%s  PASS%s %s\n", TEST_GREEN, TEST_RESET, name);
	}
	else
		ft_printf("%s  FAIL%s %s (got %d, expected %d)\n", TEST_RED,
			TEST_RESET, name, result, expected);
}

static int child_parse_with_null_settings(void)
{
	char *argv[] = {"ft_script", NULL};

	return (parsing(1, argv, NULL));
}

static int child_parse_with_zero_argc(void)
{
	t_script_settings data;

	ft_bzero(&data, sizeof(data));
	return (parsing(0, NULL, &data));
}

static int child_check_null_numeric(void)
{
	return (is_all_numbers(NULL));
}

static void check_child_case(const char *name, int (*test_function)(void),
	int expected)
{
	pid_t pid;
	int child_status;
	int result;

	pid = fork();
	if (pid == 0)
		_exit(test_function());
	if (pid < 0 || waitpid(pid, &child_status, 0) < 0)
	{
		check_return_case(name, FAILED, expected);
		return ;
	}
	if (WIFEXITED(child_status))
		result = WEXITSTATUS(child_status);
	else
	{
		tests_run++;
		ft_printf("%s  FAIL%s %s (child terminated by signal)\n", TEST_RED,
			TEST_RESET, name);
		return ;
	}
	check_return_case(name, result, expected);
}

static void run_basic_tests(void)
{
	char *empty[] = {"ft_script", NULL};
	char *target[] = {"ft_script", "session.log", NULL};
	char *second_target[] = {"ft_script", "one.log", "two.log", NULL};
	char *empty_target[] = {"ft_script", "", NULL};
	char *dash_target[] = {"ft_script", "-", NULL};

	parse_case("01 default target", 1, empty, 0, "typescript", NULL, 0, 0, 0, false);
	parse_case("02 custom target", 2, target, 0, "session.log", NULL, 0, 0, 0, false);
	parse_case("03 second target rejected", 3, second_target, 1, "one.log", NULL, 0, 0, 0, false);
	parse_case("04 empty target", 2, empty_target, 0, "", NULL, 0, 0, 0, false);
	parse_case("05 dash target", 2, dash_target, 0, "-", NULL, 0, 0, 0, false);
}

static void run_switch_tests(void)
{
	char *cases[][3] = {
		{"ft_script", "-a", NULL}, {"ft_script", "--append", NULL},
		{"ft_script", "-e", NULL}, {"ft_script", "--return", NULL},
		{"ft_script", "-f", NULL}, {"ft_script", "--flush", NULL},
		{"ft_script", "--force", NULL}, {"ft_script", "--force", NULL},
		{"ft_script", "-q", NULL}, {"ft_script", "--quiet", NULL}
	};
	unsigned int flags[] = {FLAG_APPEND, FLAG_APPEND, FLAG_RETURN, FLAG_RETURN,
		FLAG_FLUSH, FLAG_FLUSH, FLAG_FORCE, FLAG_FORCE, FLAG_QUIET, FLAG_QUIET};
	const char *names[] = {"short -a append", "long --append append",
		"short -e return", "long --return return", "short -f flush",
		"long --flush flush", "long --force force", "second long --force force",
		"short -q quiet", "long --quiet quiet"};
	int index;

	for (index = 0; index < 10; index++)
		parse_case(names[index], 2, cases[index], 0, "typescript", NULL,
			0, 0, flags[index], false);
}

static void run_argument_tests(void)
{
	char *command_short[] = {"ft_script", "-c", "echo hello", NULL};
	char *command_long[] = {"ft_script", "--command", "ls -la", NULL};
	char *command_missing[] = {"ft_script", "-c", NULL};
	char *echo_auto[] = {"ft_script", "-E", "auto", NULL};
	char *echo_always[] = {"ft_script", "--echo", "always", NULL};
	char *echo_never[] = {"ft_script", "-E", "never", NULL};
	char *echo_bad[] = {"ft_script", "-E", "sometimes", NULL};
	char *echo_missing[] = {"ft_script", "--echo", NULL};

	parse_case("16 short command", 3, command_short, 0, "typescript", "echo hello", 0, 0, FLAG_COMMAND, false);
	parse_case("17 long command", 3, command_long, 0, "typescript", "ls -la", 0, 0, FLAG_COMMAND, false);
	parse_case("18 missing command", 2, command_missing, 1, NULL, NULL, 0, 0, 0, false);
	parse_case("19 echo auto", 3, echo_auto, 0, "typescript", NULL, 0, 0, 0, false);
	parse_case("20 echo always", 3, echo_always, 0, "typescript", NULL, 1, 0, 0, false);
	parse_case("21 echo never", 3, echo_never, 0, "typescript", NULL, 2, 0, 0, false);
	parse_case("22 invalid echo", 3, echo_bad, 1, NULL, NULL, 0, 0, 0, false);
	parse_case("23 missing echo", 2, echo_missing, 1, NULL, NULL, 0, 0, 0, false);
}

static void run_file_tests(void)
{
	char *log_io[] = {"ft_script", "-B", "io.log", NULL};
	char *log_in[] = {"ft_script", "-I", "stdin.log", NULL};
	char *log_out[] = {"ft_script", "-O", "stdout.log", NULL};
	char *log_timing[] = {"ft_script", "-T", "timing.log", NULL};
	char *log_io_missing[] = {"ft_script", "--log-io", NULL};
	char *log_in_missing[] = {"ft_script", "--log-in", NULL};
	char *log_out_missing[] = {"ft_script", "--log-out", NULL};
	char *log_timing_missing[] = {"ft_script", "--log-timing", NULL};

	parse_case("24 log io", 3, log_io, 0, "typescript", NULL, 0, 0, FLAG_LOG_IO, false);
	parse_case("25 log input", 3, log_in, 0, "typescript", NULL, 0, 0, FLAG_LOG_IN, false);
	parse_case("26 log output", 3, log_out, 0, "typescript", NULL, 0, 0, FLAG_LOG_OUT, false);
	parse_case("27 log timing", 3, log_timing, 0, "typescript", NULL, 0, 0, FLAG_LOG_TIMING, false);
	parse_case("28 missing log io", 2, log_io_missing, 1, NULL, NULL, 0, 0, 0, false);
	parse_case("29 missing log input", 2, log_in_missing, 1, NULL, NULL, 0, 0, 0, false);
	parse_case("30 missing log output", 2, log_out_missing, 1, NULL, NULL, 0, 0, 0, false);
	parse_case("31 missing log timing", 2, log_timing_missing, 1, NULL, NULL, 0, 0, 0, false);
}

static void run_timing_option_tests(void)
{
	char *short_timing[] = {"ft_script", "-t", NULL};
	char *short_timing_file[] = {"ft_script", "-ttiming.log", NULL};
	char *long_timing[] = {"ft_script", "--timing", NULL};
	char *long_timing_file[] = {"ft_script", "--timing=timing.log", NULL};
	char *long_timing_target[] = {"ft_script", "--timing", "session.log", NULL};

	parse_timing_case("short timing uses default destination", 2, short_timing,
		SUCCES, "typescript", NULL);
	parse_timing_case("short timing accepts attached file", 2, short_timing_file,
		SUCCES, "typescript", "timing.log");
	parse_timing_case("long timing uses default destination", 2, long_timing,
		SUCCES, "typescript", NULL);
	parse_timing_case("long timing accepts equals file", 2, long_timing_file,
		SUCCES, "typescript", "timing.log");
	parse_timing_case("long timing keeps following target", 3, long_timing_target,
		SUCCES, "session.log", NULL);
}

static void run_format_limit_tests(void)
{
	char *classic[] = {"ft_script", "-m", "classic", NULL};
	char *advanced[] = {"ft_script", "--logging-format", "advanced", NULL};
	char *format_bad[] = {"ft_script", "-m", "json", NULL};
	char *limit_zero[] = {"ft_script", "-o", "0", NULL};
	char *limit_small[] = {"ft_script", "--output-limit", "42", NULL};
	char *limit_large[] = {"ft_script", "-o", "4294967295", NULL};
	char *limit_bad[] = {"ft_script", "-o", "12x", NULL};
	char *limit_missing[] = {"ft_script", "--output-limit", NULL};

	parse_case("32 classic format", 3, classic, 0, "typescript", NULL, 0, 0, 0, false);
	parse_case("33 advanced format", 3, advanced, 0, "typescript", NULL, 0, 0, 0, false);
	parse_case("34 invalid format", 3, format_bad, 1, NULL, NULL, 0, 0, 0, false);
	parse_case("35 zero output limit", 3, limit_zero, 0, "typescript", NULL, 0, 0, 0, false);
	parse_case("36 small output limit", 3, limit_small, 0, "typescript", NULL, 0, 42, 0, false);
	parse_case("37 large output limit", 3, limit_large, 0, "typescript", NULL, 0, 4294967295, 0, false);
	parse_case("38 invalid output limit", 3, limit_bad, 1, NULL, NULL, 0, 0, 0, false);
	parse_case("39 missing output limit", 2, limit_missing, 1, NULL, NULL, 0, 0, 0, false);
}

static void run_control_tests(void)
{
	char *help_short[] = {"ft_script", "-h", "ignored.log", NULL};
	char *help_long[] = {"ft_script", "--help", NULL};
	char *version_short[] = {"ft_script", "-V", "ignored.log", NULL};
	char *version_long[] = {"ft_script", "--version", NULL};
	char *unknown[] = {"ft_script", "--not-an-option", NULL};
	char *unknown_short[] = {"ft_script", "-z", NULL};

	parse_case("40 help stops parsing", 3, help_short, 0, NULL, NULL, 0, 0, 0, true);
	parse_case("41 long help", 2, help_long, 0, NULL, NULL, 0, 0, 0, true);
	parse_case("42 version stops parsing", 3, version_short, 0, NULL, NULL, 0, 0, 0, true);
	parse_case("43 long version", 2, version_long, 0, NULL, NULL, 0, 0, 0, true);
	parse_case("44 unknown long option", 2, unknown, 1, NULL, NULL, 0, 0, 0, false);
	parse_case("45 unknown short option", 2, unknown_short, 1, NULL, NULL, 0, 0, 0, false);
}

static void run_combination_tests(void)
{
	char *cases[][8] = {
		{"ft_script", "-a", "-q", "a.log", NULL},
		{"ft_script", "--append", "--quiet", "b.log", NULL},
		{"ft_script", "-c", "echo", "-q", NULL},
		{"ft_script", "-E", "always", "-f", NULL},
		{"ft_script", "-B", "io", "-I", "in", NULL},
		{"ft_script", "-O", "out", "-T", "time", NULL},
		{"ft_script", "-o", "1", "-m", "advanced", NULL},
		{"ft_script", "--force", "-e", "-f", "-q", NULL},
		{"ft_script", "-a", "-c", "run", "session.log", NULL}
	};
	unsigned int flags[] = {FLAG_APPEND | FLAG_QUIET, FLAG_APPEND | FLAG_QUIET,
		FLAG_COMMAND | FLAG_QUIET, FLAG_FLUSH, FLAG_LOG_IO | FLAG_LOG_IN,
		FLAG_LOG_OUT | FLAG_LOG_TIMING, 0, FLAG_FORCE | FLAG_RETURN | FLAG_FLUSH | FLAG_QUIET,
		FLAG_APPEND | FLAG_COMMAND};
	const char *commands[] = {NULL, NULL, "echo", NULL, NULL, NULL, NULL, NULL, "run"};
	const char *targets[] = {"a.log", "b.log", "typescript", "typescript", "typescript",
		"typescript", "typescript", "typescript", "session.log"};
	int argc_values[] = {4, 4, 4, 4, 6, 6, 6, 5, 5};
	int index;

	for (index = 0; index < 9; index++)
		parse_case(index == 0 ? "append quiet with target" : index == 1 ? "long append quiet with target" :
			index == 2 ? "command followed by quiet" : index == 3 ? "echo always and flush" :
			index == 4 ? "log io and log input" : index == 5 ? "log output and timing" :
			index == 6 ? "output limit and advanced format" : index == 7 ? "force return flush quiet" :
			"append command with target", argc_values[index], cases[index], 0, targets[index],
			commands[index], index == 3 ? 1 : 0, index == 6 ? 1 : 0, flags[index], false);
}

static void run_matrix_tests(void)
{
	char *echo_values[] = {"auto", "always", "never"};
	char *format_values[] = {"classic", "advanced"};
	char *limits[] = {"7", "99", "1000", "65535", "123456", "2147483647", "42", "8", "16", "32"};
	char *argv_case[6];
	int index;

	argv_case[0] = "ft_script";
	argv_case[1] = "-E";
	for (index = 0; index < 3; index++)
	{
		argv_case[2] = echo_values[index];
		argv_case[3] = "matrix.log";
		argv_case[4] = NULL;
		parse_case(index == 0 ? "echo auto with target" : index == 1 ? "echo always with target" :
			"echo never with target", 4, argv_case, 0, "matrix.log", NULL,
			index, 0, 0, false);
	}
	argv_case[1] = "-m";
	for (index = 0; index < 2; index++)
	{
		argv_case[2] = format_values[index];
		argv_case[3] = "format.log";
		argv_case[4] = NULL;
		parse_case(index == 0 ? "classic format with target" : "advanced format with target",
			4, argv_case, 0, "format.log", NULL,
			0, 0, 0, false);
	}
	argv_case[1] = "-o";
	for (index = 0; index < 10; index++)
	{
		argv_case[2] = limits[index];
		argv_case[3] = "limit.log";
		argv_case[4] = NULL;
		parse_case("numeric output limit with target", 4, argv_case, 0, "limit.log", NULL, 0,
			ft_atoll(limits[index]), 0, false);
	}
}

static void run_extra_tests(void)
{
	char *targets[] = {"one", "two.log", "typescript", "a/b.log", ".hidden",
		"file with spaces", "123", "dash-x", "UPPER", "last.log"};
	char *switches[] = {"-a", "--append", "-e", "--return", "-f", "--flush",
		"--force", "-q", "--quiet", "-a"};
	unsigned int switch_flags[] = {FLAG_APPEND, FLAG_APPEND, FLAG_RETURN, FLAG_RETURN,
		FLAG_FLUSH, FLAG_FLUSH, FLAG_FORCE, FLAG_QUIET, FLAG_QUIET, FLAG_APPEND};
	char *log_cases[][3] = {
		{"ft_script", "--log-io", "all.log"},
		{"ft_script", "--log-in", "in.log"},
		{"ft_script", "--log-out", "out.log"},
		{"ft_script", "--log-timing", "time.log"},
		{"ft_script", "-I", "input.log"},
		{"ft_script", "-O", "output.log"}
	};
	unsigned int log_flags[] = {FLAG_LOG_IO, FLAG_LOG_IN, FLAG_LOG_OUT,
		FLAG_LOG_TIMING, FLAG_LOG_IN, FLAG_LOG_OUT};
	char *limits[] = {"1", "2", "10", "999999", "2147483648"};
	char *limit_cases[5][4];
	int index;

	for (index = 0; index < 10; index++)
	{
		parse_case("target path and filename", 2, (char *[]){"ft_script", targets[index], NULL},
			0, targets[index], NULL, 0, 0, 0, false);
	}
	for (index = 0; index < 10; index++)
		parse_case("repeated boolean switch alias", 2, (char *[]){"ft_script", switches[index], NULL},
			0, "typescript", NULL, 0, 0, switch_flags[index], false);
	for (index = 0; index < 6; index++)
		parse_case("long and short log destination", 3, log_cases[index], 0, "typescript", NULL,
			0, 0, log_flags[index], false);
	for (index = 0; index < 5; index++)
	{
		limit_cases[index][0] = "ft_script";
		limit_cases[index][1] = "-o";
		limit_cases[index][2] = limits[index];
		limit_cases[index][3] = NULL;
		parse_case("boundary numeric output limit", 3, limit_cases[index], 0, "typescript", NULL,
			0, ft_atoll(limits[index]), 0, false);
	}
}

static void run_invalid_input_tests(void)
{
	char *empty_limit[] = {"ft_script", "-o", "", NULL};
	char *negative_limit[] = {"ft_script", "--output-limit", "-1", NULL};
	char *signed_limit[] = {"ft_script", "-o", "+10", NULL};
	char *empty_echo[] = {"ft_script", "-E", "", NULL};
	char *empty_format[] = {"ft_script", "-m", "", NULL};
	char *double_dash[] = {"ft_script", "--", NULL};
	char *unknown_short[] = {"ft_script", "-z", NULL};
	char *unknown_long[] = {"ft_script", "--unknown-option", NULL};

	parse_case("empty output limit is rejected", 3, empty_limit, FAILED, NULL,
		NULL, ECHO_AUTO, 0, 0, false);
	parse_case("negative output limit is rejected", 3, negative_limit, FAILED,
		NULL, NULL, ECHO_AUTO, 0, 0, false);
	parse_case("signed output limit is rejected", 3, signed_limit, FAILED, NULL,
		NULL, ECHO_AUTO, 0, 0, false);
	parse_case("empty echo mode is rejected", 3, empty_echo, FAILED, NULL, NULL,
		ECHO_AUTO, 0, 0, false);
	parse_case("empty logging format is rejected", 3, empty_format, FAILED, NULL,
		NULL, ECHO_AUTO, 0, 0, false);
	parse_case("bare double dash is rejected", 2, double_dash, FAILED, NULL, NULL,
		ECHO_AUTO, 0, 0, false);
	parse_case("unknown short option is rejected", 2, unknown_short, FAILED, NULL,
		NULL, ECHO_AUTO, 0, 0, false);
	parse_case("unknown long option is rejected", 2, unknown_long, FAILED, NULL,
		NULL, ECHO_AUTO, 0, 0, false);
}

static void run_api_safety_tests(void)
{
	t_script_settings data;
	char *program_only[] = {"ft_script", NULL};

	ft_bzero(&data, sizeof(data));
	check_child_case("argc zero does not crash", child_parse_with_zero_argc, FAILED);
	check_return_case("NULL argv is rejected safely", parsing(1, NULL, &data), FAILED);
	check_child_case("NULL settings does not crash", child_parse_with_null_settings, FAILED);
	check_return_case("program name alone parses safely", parsing(1, program_only, &data), SUCCES);
	check_return_case("empty string is not a flag", is_flags(""), false);
	check_child_case("NULL string does not crash numeric check", child_check_null_numeric, false);
	check_return_case("empty string is not numeric", is_all_numbers(""), false);
}

int main(void)
{
	parsing_test_fd = open("tests/parsing_output.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (parsing_test_fd < 0)
		return (1);
	ft_printf("%s\n🧪 %sint	parsing(int argc, char **argv, t_script_settings *data) unit tests%s\n", TEST_CYAN, TEST_YELLOW, TEST_RESET);
	run_basic_tests();
	run_switch_tests();
	run_argument_tests();
	run_file_tests();
	run_timing_option_tests();
	run_format_limit_tests();
	run_control_tests();
	run_combination_tests();
	run_matrix_tests();
	run_extra_tests();
	run_invalid_input_tests();
	run_api_safety_tests();
	ft_printf("\n%s%d/%d tests passed%s ", tests_passed == tests_run ? TEST_GREEN : TEST_RED,
		tests_passed, tests_run, TEST_RESET);
	if (tests_passed == tests_run)
		ft_printf("✅\n");
	else
		ft_printf("❌\n");
	close(parsing_test_fd);
	return (tests_passed == tests_run ? 0 : 1);
}