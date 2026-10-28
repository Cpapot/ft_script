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
#define FLAG_LOG_IN (1u << 5)
#define FLAG_LOG_TIMING (1u << 6)
#define FLAG_QUIET (1u << 7)

static int tests_run;
static int tests_passed;
int parsing_test_fd;

typedef struct s_parse_snapshot
{
	int		result;
	int		echo_mode;
	off_t	output_limit;
	bool	is_append;
	bool	is_return;
	bool	is_flush;
	bool	is_force;
	bool	is_quiet;
	bool	is_closing;
	bool	has_target;
	bool	has_command;
	bool	has_log_in;
	bool	has_log_timing;
	char	target[256];
	char	command[256];
	char	log_io[256];
	char	log_in[256];
	char	log_timing[256];
	char	timing[256];
	bool	has_timing;
} t_parse_snapshot;

typedef struct s_parser_test_case
{
	const char		*name;
	int				argc;
	char			**argv;
	int				expected_return;
	const char		*expected_target;
	const char		*expected_command;
	int				expected_echo_mode;
	off_t			expected_output_limit;
	unsigned int	expected_flags;
	bool			expected_closing;
} t_parser_test_case;

typedef struct s_timing_test_case
{
	const char	*name;
	int			argc;
	char		**argv;
	int			expected_return;
	const char	*expected_target;
	const char	*expected_timing;
} t_timing_test_case;

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
	if (data->logIn != NULL)
		actual |= FLAG_LOG_IN;
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
	if (data->logIn != NULL)
		flags |= FLAG_LOG_IN;
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
	if (!same_string(data->logOut, target))
		ft_printf("      target: got %s, expected %s\n", data->logOut, target);
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

static void snapshot_string(char *destination, size_t size, char *source,
	bool *present)
{
	if (source == NULL)
	{
		*present = false;
		return ;
	}
	*present = true;
	ft_strlcpy(destination, source, size);
}

static void write_snapshot(int fd, int result, t_script_settings *data)
{
	t_parse_snapshot snapshot;

	ft_bzero(&snapshot, sizeof(snapshot));
	snapshot.result = result;
	snapshot.echo_mode = data->echoMode;
	snapshot.output_limit = data->outputLimit;
	snapshot.is_append = data->isAppend;
	snapshot.is_return = data->isReturn;
	snapshot.is_flush = data->isFlush;
	snapshot.is_force = data->isForce;
	snapshot.is_quiet = data->isQuiet;
	snapshot.is_closing = data->isClosing;
	snapshot_string(snapshot.target, sizeof(snapshot.target), data->logOut,
		&snapshot.has_target);
	snapshot_string(snapshot.command, sizeof(snapshot.command), data->command,
		&snapshot.has_command);
	snapshot_string(snapshot.log_in, sizeof(snapshot.log_in), data->logIn,
		&snapshot.has_log_in);
	snapshot_string(snapshot.log_timing, sizeof(snapshot.log_timing), data->logTiming,
		&snapshot.has_log_timing);
	snapshot_string(snapshot.timing, sizeof(snapshot.timing), data->timing,
		&snapshot.has_timing);
	write(fd, &snapshot, sizeof(snapshot));
}

static int read_snapshot(int fd, t_parse_snapshot *snapshot)
{
	ssize_t received;
	size_t total;

	total = 0;
	while (total < sizeof(*snapshot))
	{
		received = read(fd, (char *)snapshot + total, sizeof(*snapshot) - total);
		if (received <= 0)
			return (0);
		total += received;
	}
	return (1);
}

static void restore_snapshot(t_script_settings *data,
	t_parse_snapshot *snapshot)
{
	data->echoMode = snapshot->echo_mode;
	data->outputLimit = snapshot->output_limit;
	data->isAppend = snapshot->is_append;
	data->isReturn = snapshot->is_return;
	data->isFlush = snapshot->is_flush;
	data->isForce = snapshot->is_force;
	data->isQuiet = snapshot->is_quiet;
	data->isClosing = snapshot->is_closing;
	data->logOut = snapshot->has_target ? snapshot->target : NULL;
	data->command = snapshot->has_command ? snapshot->command : NULL;
	data->logIn = snapshot->has_log_in ? snapshot->log_in : NULL;
	data->logTiming = snapshot->has_log_timing ? snapshot->log_timing : NULL;
	data->timing = snapshot->has_timing ? snapshot->timing : NULL;
}

static void check_return_case(const char *name, int result, int expected);
static void parse_case(const char *name, int argc, char **argv, int expected_ret,
	const char *target, const char *command, int echo_mode, off_t output_limit,
	unsigned int flags, bool closing);
static void parse_timing_case(const char *name, int argc, char **argv,
	int expected_ret, const char *target, const char *timing);

static void run_parser_test_case(t_parser_test_case test)
{
	parse_case(test.name, test.argc, test.argv, test.expected_return,
		test.expected_target, test.expected_command,
		test.expected_echo_mode, test.expected_output_limit,
		test.expected_flags, test.expected_closing);
}

static void run_timing_test_case(t_timing_test_case test)
{
	parse_timing_case(test.name, test.argc, test.argv, test.expected_return,
		test.expected_target, test.expected_timing);
}

static void parse_case(const char *name, int argc, char **argv, int expected_ret,
	const char *target, const char *command, int echo_mode, off_t output_limit,
	unsigned int flags, bool closing)
{
	int snapshot_pipe[2];
	pid_t pid;
	int child_status;
	t_parse_snapshot snapshot;
	t_script_settings data;
	int valid;

	if (pipe(snapshot_pipe) < 0)
	{
		check_return_case(name, FAILED, expected_ret);
		return ;
	}
	pid = fork();
	if (pid == 0)
	{
		int result;

		ft_bzero(&data, sizeof(data));
		close(snapshot_pipe[0]);
		result = parsing(argc, argv, &data);
		write_snapshot(snapshot_pipe[1], result, &data);
		close(snapshot_pipe[1]);
		_exit(0);
	}
	close(snapshot_pipe[1]);
	if (pid < 0 || !read_snapshot(snapshot_pipe[0], &snapshot)
		|| waitpid(pid, &child_status, 0) < 0)
	{
		close(snapshot_pipe[0]);
		tests_run++;
		ft_printf("%s  FAIL%s %s (parser crashed or could not be read)\n",
			TEST_RED, TEST_RESET, name);
		print_input(argc, argv);
		return ;
	}
	close(snapshot_pipe[0]);
	if (!WIFEXITED(child_status))
	{
		tests_run++;
		ft_printf("%s  FAIL%s %s (parser terminated by signal)\n", TEST_RED,
			TEST_RESET, name);
		print_input(argc, argv);
		return ;
	}
	ft_bzero(&data, sizeof(data));
	restore_snapshot(&data, &snapshot);
	valid = snapshot.result == expected_ret;
	valid = valid && same_string(data.logOut, target);
	valid = valid && same_string(data.command, command);
	valid = valid && data.echoMode == echo_mode;
	valid = valid && data.outputLimit == output_limit;
	valid = valid && has_flags(&data, flags);
	valid = valid && data.isClosing == closing;
	check_case(name, valid, 1, argc, argv, snapshot.result, expected_ret, &data, target,
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
	valid = valid && same_string(data.logOut, target);
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

static int child_parse_with_null_argv(void)
{
	t_script_settings data;

	ft_bzero(&data, sizeof(data));
	return (parsing(1, NULL, &data));
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
	char *argv_default[] = {"ft_script", NULL};
	char *argv_target[] = {"ft_script", "session.log", NULL};
	char *argv_two_targets[] = {"ft_script", "one.log", "two.log", NULL};
	char *argv_empty_target[] = {"ft_script", "", NULL};
	char *argv_dash_target[] = {"ft_script", "-", NULL};
	t_parser_test_case cases[] = {
		{"default target", 1, argv_default, SUCCES, "typescript", NULL, ECHO_AUTO, 0, 0, false},
		{"custom target", 2, argv_target, SUCCES, "session.log", NULL, ECHO_AUTO, 0, 0, false},
		{"two targets rejected", 3, argv_two_targets, FAILED, "one.log", NULL, ECHO_AUTO, 0, 0, false},
		{"empty target", 2, argv_empty_target, SUCCES, "", NULL, ECHO_AUTO, 0, 0, false},
		{"dash target", 2, argv_dash_target, SUCCES, "-", NULL, ECHO_AUTO, 0, 0, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_switch_tests(void)
{
	char *argv_short_append[] = {"ft_script", "-a", NULL};
	char *argv_long_append[] = {"ft_script", "--append", NULL};
	char *argv_short_return[] = {"ft_script", "-e", NULL};
	char *argv_long_return[] = {"ft_script", "--return", NULL};
	char *argv_short_flush[] = {"ft_script", "-f", NULL};
	char *argv_long_flush[] = {"ft_script", "--flush", NULL};
	char *argv_force[] = {"ft_script", "--force", NULL};
	char *argv_short_quiet[] = {"ft_script", "-q", NULL};
	char *argv_long_quiet[] = {"ft_script", "--quiet", NULL};
	t_parser_test_case cases[] = {
		{"short -a append", 2, argv_short_append, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_APPEND, false},
		{"long --append append", 2, argv_long_append, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_APPEND, false},
		{"short -e return", 2, argv_short_return, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_RETURN, false},
		{"long --return return", 2, argv_long_return, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_RETURN, false},
		{"short -f flush", 2, argv_short_flush, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_FLUSH, false},
		{"long --flush flush", 2, argv_long_flush, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_FLUSH, false},
		{"long --force force", 2, argv_force, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_FORCE, false},
		{"short -q quiet", 2, argv_short_quiet, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_QUIET, false},
		{"long --quiet quiet", 2, argv_long_quiet, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_QUIET, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_argument_tests(void)
{
	char *argv_command_short[] = {"ft_script", "-c", "echo hello", NULL};
	char *argv_command_long[] = {"ft_script", "--command", "ls -la", NULL};
	char *argv_command_missing[] = {"ft_script", "-c", NULL};
	char *argv_echo_auto[] = {"ft_script", "-E", "auto", NULL};
	char *argv_echo_always[] = {"ft_script", "--echo", "always", NULL};
	char *argv_echo_never[] = {"ft_script", "-E", "never", NULL};
	char *argv_echo_bad[] = {"ft_script", "-E", "sometimes", NULL};
	char *argv_echo_missing[] = {"ft_script", "--echo", NULL};
	t_parser_test_case cases[] = {
		{"short command", 3, argv_command_short, SUCCES, "typescript", "echo hello", ECHO_AUTO, 0, FLAG_COMMAND, false},
		{"long command", 3, argv_command_long, SUCCES, "typescript", "ls -la", ECHO_AUTO, 0, FLAG_COMMAND, false},
		{"missing command", 2, argv_command_missing, FAILED, NULL, NULL, ECHO_AUTO, 0, 0, false},
		{"echo auto", 3, argv_echo_auto, SUCCES, "typescript", NULL, ECHO_AUTO, 0, 0, false},
		{"echo always", 3, argv_echo_always, SUCCES, "typescript", NULL, ECHO_ALWAYS, 0, 0, false},
		{"echo never", 3, argv_echo_never, SUCCES, "typescript", NULL, ECHO_NEVER, 0, 0, false},
		{"invalid echo", 3, argv_echo_bad, FAILED, NULL, NULL, ECHO_AUTO, 0, 0, false},
		{"missing echo", 2, argv_echo_missing, FAILED, NULL, NULL, ECHO_AUTO, 0, 0, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_file_tests(void)
{
	char *log_io[] = {"ft_script", "-B", "io.log", NULL};
	char *log_in[] = {"ft_script", "-I", "stdin.log", NULL};
	char *log_timing[] = {"ft_script", "-T", "timing.log", NULL};
	char *output_target[] = {"ft_script", "-O", "ok", NULL};
	char *output_target_twice[] = {"ft_script", "-O", "ok", "ok", NULL};
	char *log_io_missing[] = {"ft_script", "--log-io", NULL};
	char *log_in_missing[] = {"ft_script", "--log-in", NULL};
	char *log_timing_missing[] = {"ft_script", "--log-timing", NULL};
	t_parser_test_case cases[] = {
		{"-B sets output target", 3, log_io, SUCCES, "io.log", NULL,
			ECHO_AUTO, 0, FLAG_LOG_IN, false},
		{"-I has no output target", 3, log_in, SUCCES, NULL, NULL,
			ECHO_AUTO, 0, FLAG_LOG_IN, false},
		{"-T sets timing log", 3, log_timing, SUCCES, "typescript", NULL,
			ECHO_AUTO, 0, FLAG_LOG_TIMING, false},
		{"-O sets output target", 3, output_target, SUCCES, "ok", NULL,
			ECHO_AUTO, 0, 0, false},
		{"-O rejects two targets", 4, output_target_twice, FAILED, "ok", NULL,
			ECHO_AUTO, 0, 0, false},
		{"--log-io requires a file", 2, log_io_missing, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"--log-in requires a file", 2, log_in_missing, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"--log-timing requires a file", 2, log_timing_missing, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_timing_option_tests(void)
{
	char *short_timing[] = {"ft_script", "-t", NULL};
	char *short_timing_file[] = {"ft_script", "-ttiming.log", NULL};
	char *long_timing[] = {"ft_script", "--timing", NULL};
	char *long_timing_file[] = {"ft_script", "--timing=timing.log", NULL};
	char *long_timing_target[] = {"ft_script", "--timing", "session.log", NULL};

	t_timing_test_case cases[] = {
		{"short timing default destination", 2, short_timing, SUCCES, "typescript", NULL},
		{"short timing attached file", 2, short_timing_file, SUCCES, "typescript", "timing.log"},
		{"long timing default destination", 2, long_timing, SUCCES, "typescript", NULL},
		{"long timing equals file", 2, long_timing_file, SUCCES, "typescript", "timing.log"},
		{"long timing followed by target", 3, long_timing_target, SUCCES, "session.log", NULL}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_timing_test_case(cases[index]);
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

	t_parser_test_case cases[] = {
		{"classic logging format", 3, classic, SUCCES, "typescript", NULL, ECHO_AUTO, 0, 0, false},
		{"advanced logging format", 3, advanced, SUCCES, "typescript", NULL, ECHO_AUTO, 0, 0, false},
		{"invalid logging format", 3, format_bad, FAILED, NULL, NULL, ECHO_AUTO, 0, 0, false},
		{"zero output limit", 3, limit_zero, SUCCES, "typescript", NULL, ECHO_AUTO, 0, 0, false},
		{"small output limit", 3, limit_small, SUCCES, "typescript", NULL, ECHO_AUTO, 42, 0, false},
		{"large output limit", 3, limit_large, SUCCES, "typescript", NULL, ECHO_AUTO, 4294967295, 0, false},
		{"invalid output limit", 3, limit_bad, FAILED, NULL, NULL, ECHO_AUTO, 0, 0, false},
		{"missing output limit", 2, limit_missing, FAILED, NULL, NULL, ECHO_AUTO, 0, 0, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_control_tests(void)
{
	char *help_short[] = {"ft_script", "-h", "ignored.log", NULL};
	char *help_long[] = {"ft_script", "--help", NULL};
	char *version_short[] = {"ft_script", "-V", "ignored.log", NULL};
	char *version_long[] = {"ft_script", "--version", NULL};
	char *unknown[] = {"ft_script", "--not-an-option", NULL};
	char *unknown_short[] = {"ft_script", "-z", NULL};

	t_parser_test_case cases[] = {
		{"short help stops parsing", 3, help_short, SUCCES, NULL, NULL, ECHO_AUTO, 0, 0, true},
		{"long help stops parsing", 2, help_long, SUCCES, NULL, NULL, ECHO_AUTO, 0, 0, true},
		{"short version stops parsing", 3, version_short, SUCCES, NULL, NULL, ECHO_AUTO, 0, 0, true},
		{"long version stops parsing", 2, version_long, SUCCES, NULL, NULL, ECHO_AUTO, 0, 0, true},
		{"unknown long option", 2, unknown, FAILED, NULL, NULL, ECHO_AUTO, 0, 0, false},
		{"unknown short option", 2, unknown_short, FAILED, NULL, NULL, ECHO_AUTO, 0, 0, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_combination_tests(void)
{
	char *argv_append_quiet[] = {"ft_script", "-a", "-q", "a.log", NULL};
	char *argv_long_append_quiet[] = {"ft_script", "--append", "--quiet", "b.log", NULL};
	char *argv_command_quiet[] = {"ft_script", "-c", "echo", "-q", NULL};
	char *argv_echo_flush[] = {"ft_script", "-E", "always", "-f", NULL};
	char *argv_io_input[] = {"ft_script", "-B", "io", "-I", "in", NULL};
	char *argv_all_outputs[] = {"ft_script", "-I", "in", "-B", "both", "-O", "out", NULL};
	char *argv_output_timing[] = {"ft_script", "-O", "out", "-T", "time", NULL};
	char *argv_limit_format[] = {"ft_script", "-o", "1", "-m", "advanced", NULL};
	char *argv_force_return[] = {"ft_script", "--force", "-e", "-f", "-q", NULL};
	char *argv_command_target[] = {"ft_script", "-a", "-c", "run", "session.log", NULL};
	t_parser_test_case cases[] = {
		{"append and quiet", 4, argv_append_quiet, SUCCES, "a.log", NULL, ECHO_AUTO, 0, FLAG_APPEND | FLAG_QUIET, false},
		{"long append and quiet", 4, argv_long_append_quiet, SUCCES, "b.log", NULL, ECHO_AUTO, 0, FLAG_APPEND | FLAG_QUIET, false},
		{"command and quiet", 4, argv_command_quiet, SUCCES, "typescript", "echo", ECHO_AUTO, 0, FLAG_COMMAND | FLAG_QUIET, false},
		{"echo always and flush", 4, argv_echo_flush, SUCCES, "typescript", NULL, ECHO_ALWAYS, 0, FLAG_FLUSH, false},
		{"log io and input", 5, argv_io_input, SUCCES, "io", NULL, ECHO_AUTO, 0, FLAG_LOG_IN, false},
		{"input, io, and output combined", 7, argv_all_outputs, SUCCES, "out", NULL, ECHO_AUTO, 0, FLAG_LOG_IN, false},
		{"output target and timing", 5, argv_output_timing, SUCCES, "out", NULL, ECHO_AUTO, 0, FLAG_LOG_TIMING, false},
		{"output limit and format", 6, argv_limit_format, SUCCES, "typescript", NULL, ECHO_AUTO, 1, 0, false},
		{"force, return, flush, quiet", 5, argv_force_return, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_FORCE | FLAG_RETURN | FLAG_FLUSH | FLAG_QUIET, false},
		{"append command and target", 5, argv_command_target, SUCCES, "session.log", "run", ECHO_AUTO, 0, FLAG_APPEND | FLAG_COMMAND, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_matrix_tests(void)
{
	char *argv_echo_auto[] = {"ft_script", "-E", "auto", "matrix.log", NULL};
	char *argv_echo_always[] = {"ft_script", "-E", "always", "matrix.log", NULL};
	char *argv_echo_never[] = {"ft_script", "-E", "never", "matrix.log", NULL};
	char *argv_classic[] = {"ft_script", "-m", "classic", "format.log", NULL};
	char *argv_advanced[] = {"ft_script", "-m", "advanced", "format.log", NULL};
	char *argv_limit_7[] = {"ft_script", "-o", "7", "limit.log", NULL};
	char *argv_limit_99[] = {"ft_script", "-o", "99", "limit.log", NULL};
	char *argv_limit_1000[] = {"ft_script", "-o", "1000", "limit.log", NULL};
	char *argv_limit_65535[] = {"ft_script", "-o", "65535", "limit.log", NULL};
	char *argv_limit_123456[] = {"ft_script", "-o", "123456", "limit.log", NULL};
	char *argv_limit_2147483647[] = {"ft_script", "-o", "2147483647", "limit.log", NULL};
	char *argv_limit_42[] = {"ft_script", "-o", "42", "limit.log", NULL};
	char *argv_limit_8[] = {"ft_script", "-o", "8", "limit.log", NULL};
	char *argv_limit_16[] = {"ft_script", "-o", "16", "limit.log", NULL};
	char *argv_limit_32[] = {"ft_script", "-o", "32", "limit.log", NULL};
	t_parser_test_case cases[] = {
		{"echo auto with target", 4, argv_echo_auto, SUCCES, "matrix.log", NULL, ECHO_AUTO, 0, 0, false},
		{"echo always with target", 4, argv_echo_always, SUCCES, "matrix.log", NULL, ECHO_ALWAYS, 0, 0, false},
		{"echo never with target", 4, argv_echo_never, SUCCES, "matrix.log", NULL, ECHO_NEVER, 0, 0, false},
		{"classic format with target", 4, argv_classic, SUCCES, "format.log", NULL, ECHO_AUTO, 0, 0, false},
		{"advanced format with target", 4, argv_advanced, SUCCES, "format.log", NULL, ECHO_AUTO, 0, 0, false},
		{"limit 7 with target", 4, argv_limit_7, SUCCES, "limit.log", NULL, ECHO_AUTO, 7, 0, false},
		{"limit 99 with target", 4, argv_limit_99, SUCCES, "limit.log", NULL, ECHO_AUTO, 99, 0, false},
		{"limit 1000 with target", 4, argv_limit_1000, SUCCES, "limit.log", NULL, ECHO_AUTO, 1000, 0, false},
		{"limit 65535 with target", 4, argv_limit_65535, SUCCES, "limit.log", NULL, ECHO_AUTO, 65535, 0, false},
		{"limit 123456 with target", 4, argv_limit_123456, SUCCES, "limit.log", NULL, ECHO_AUTO, 123456, 0, false},
		{"limit 2147483647 with target", 4, argv_limit_2147483647, SUCCES, "limit.log", NULL, ECHO_AUTO, 2147483647, 0, false},
		{"limit 42 with target", 4, argv_limit_42, SUCCES, "limit.log", NULL, ECHO_AUTO, 42, 0, false},
		{"limit 8 with target", 4, argv_limit_8, SUCCES, "limit.log", NULL, ECHO_AUTO, 8, 0, false},
		{"limit 16 with target", 4, argv_limit_16, SUCCES, "limit.log", NULL, ECHO_AUTO, 16, 0, false},
		{"limit 32 with target", 4, argv_limit_32, SUCCES, "limit.log", NULL, ECHO_AUTO, 32, 0, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_extra_tests(void)
{
	char *argv_one[] = {"ft_script", "one", NULL};
	char *argv_two[] = {"ft_script", "two.log", NULL};
	char *argv_typescript[] = {"ft_script", "typescript", NULL};
	char *argv_path[] = {"ft_script", "a/b.log", NULL};
	char *argv_hidden[] = {"ft_script", ".hidden", NULL};
	char *argv_spaces[] = {"ft_script", "file with spaces", NULL};
	char *argv_number[] = {"ft_script", "123", NULL};
	char *argv_dash_name[] = {"ft_script", "dash-x", NULL};
	char *argv_upper[] = {"ft_script", "UPPER", NULL};
	char *argv_last[] = {"ft_script", "last.log", NULL};
	char *argv_a[] = {"ft_script", "-a", NULL};
	char *argv_append[] = {"ft_script", "--append", NULL};
	char *argv_e[] = {"ft_script", "-e", NULL};
	char *argv_return[] = {"ft_script", "--return", NULL};
	char *argv_f[] = {"ft_script", "-f", NULL};
	char *argv_flush[] = {"ft_script", "--flush", NULL};
	char *argv_force[] = {"ft_script", "--force", NULL};
	char *argv_q[] = {"ft_script", "-q", NULL};
	char *argv_quiet[] = {"ft_script", "--quiet", NULL};
	char *argv_log_io[] = {"ft_script", "--log-io", "all.log", NULL};
	char *argv_log_in[] = {"ft_script", "--log-in", "in.log", NULL};
	char *argv_log_timing[] = {"ft_script", "--log-timing", "time.log", NULL};
	char *argv_input[] = {"ft_script", "-I", "input.log", NULL};
	char *argv_output[] = {"ft_script", "-O", "output.log", NULL};
	t_parser_test_case cases[] = {
		{"target one", 2, argv_one, SUCCES, "one", NULL, ECHO_AUTO, 0, 0, false},
		{"target two.log", 2, argv_two, SUCCES, "two.log", NULL, ECHO_AUTO, 0, 0, false},
		{"target typescript", 2, argv_typescript, SUCCES, "typescript", NULL, ECHO_AUTO, 0, 0, false},
		{"target nested path", 2, argv_path, SUCCES, "a/b.log", NULL, ECHO_AUTO, 0, 0, false},
		{"target hidden file", 2, argv_hidden, SUCCES, ".hidden", NULL, ECHO_AUTO, 0, 0, false},
		{"target with spaces", 2, argv_spaces, SUCCES, "file with spaces", NULL, ECHO_AUTO, 0, 0, false},
		{"numeric target", 2, argv_number, SUCCES, "123", NULL, ECHO_AUTO, 0, 0, false},
		{"dash-named target", 2, argv_dash_name, SUCCES, "dash-x", NULL, ECHO_AUTO, 0, 0, false},
		{"uppercase target", 2, argv_upper, SUCCES, "UPPER", NULL, ECHO_AUTO, 0, 0, false},
		{"last target", 2, argv_last, SUCCES, "last.log", NULL, ECHO_AUTO, 0, 0, false},
		{"-a append", 2, argv_a, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_APPEND, false},
		{"--append append", 2, argv_append, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_APPEND, false},
		{"-e return", 2, argv_e, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_RETURN, false},
		{"--return return", 2, argv_return, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_RETURN, false},
		{"-f flush", 2, argv_f, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_FLUSH, false},
		{"--flush flush", 2, argv_flush, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_FLUSH, false},
		{"--force force", 2, argv_force, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_FORCE, false},
		{"-q quiet", 2, argv_q, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_QUIET, false},
		{"--quiet quiet", 2, argv_quiet, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_QUIET, false},
		{"--log-io destination", 3, argv_log_io, SUCCES, "all.log", NULL, ECHO_AUTO, 0, FLAG_LOG_IN, false},
		{"--log-in without output", 3, argv_log_in, SUCCES, NULL, NULL, ECHO_AUTO, 0, FLAG_LOG_IN, false},
		{"--log-timing destination", 3, argv_log_timing, SUCCES, "typescript", NULL, ECHO_AUTO, 0, FLAG_LOG_TIMING, false},
		{"-I without output", 3, argv_input, SUCCES, NULL, NULL, ECHO_AUTO, 0, FLAG_LOG_IN, false},
		{"-O output destination", 3, argv_output, SUCCES, "output.log", NULL, ECHO_AUTO, 0, 0, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
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
	t_parser_test_case cases[] = {
		{"empty output limit", 3, empty_limit, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"negative output limit", 3, negative_limit, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"signed output limit", 3, signed_limit, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"empty echo mode", 3, empty_echo, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"empty logging format", 3, empty_format, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"bare double dash", 2, double_dash, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"unknown short option", 2, unknown_short, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false},
		{"unknown long option", 2, unknown_long, FAILED, NULL, NULL,
			ECHO_AUTO, 0, 0, false}
	};
	int index;

	for (index = 0; index < (int)(sizeof(cases) / sizeof(*cases)); index++)
		run_parser_test_case(cases[index]);
}

static void run_api_safety_tests(void)
{
	t_script_settings data;
	char *program_only[] = {"ft_script", NULL};

	ft_bzero(&data, sizeof(data));
	check_child_case("argc zero does not crash", child_parse_with_zero_argc, FAILED);
	check_child_case("NULL argv does not crash", child_parse_with_null_argv, FAILED);
	check_child_case("NULL settings does not crash", child_parse_with_null_settings, FAILED);
	check_return_case("program name alone parses safely", parsing(1, program_only, &data), SUCCES);
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