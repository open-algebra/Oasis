# ##############################################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# cmake-format.py
# ##############################################################################

# Defines options affecting file parsing.
with section("parse"):
    # Provides the structure of custom CMake functions.
    additional_commands = {}

    # Provides configuration overrides per-command where available.
    override_spec = {}

    # Provides variable tags.
    vartags = []

    # Provides property tags.
    proptags = []

# Defines options affecting formatting.
with section("format"):
    # Determines whether to format.
    disable = False

    # Specifies how wide to allow formatted CMake files.
    line_width = 80

    # Specifies how many spaces to tab for indent.
    tab_size = 4

    # Determines whether to use tabs or spaces for indenting.
    # If true, lines are indented using tabs. Else, spaces.
    # In cases where the layout would require a fractional tab character,
    # behavior is governed by <fractional_tab_policy>.
    use_tabchars = False

    # Used only when <use_tabchars> is true.
    # Indicates how fractional indentations are handled.
    # If set to 'use-space', fractional indentation is left as spaces.
    # If set to 'round-up', fractional indentation is replaced with a tab.
    fractional_tab_policy = 'use-space'

    # Specifies the maximum number of argument sub-groups allowed to wrap.
    # If an argument group contains more than this many sub-groups, then forces
    # it to a vertical layout.
    max_subgroups_hwrap = 2

    # Specifies the maximum number of positional arguments allowed to wrap.
    # If a positional argument group contains more than this many arguments,
    # then forces it to a vertical layout.
    max_pargs_hwrap = 6

    # Specifies the maximum number of lines a cmdline positional group can use.
    # If a cmdline positional group uses more than this many lines without
    # nesting, then invalidates the layout.
    max_rows_cmdline = 2

    # Determines whether to separate flow control names from their parentheses.
    # If true, separates using a space.
    separate_ctrl_name_with_space = False

    # Determines whether to separate function names from their parentheses.
    # If true, separates using a space.
    separate_fn_name_with_space = False

    # Determines whether to dangle a closing parenthesis on its own line.
    # If true, dangles the closing oarenthesis if a statement is wrapped to
    # more than one line.
    dangle_parens = False

    # Indicates how dangling parentheses are handled.
    # If set to 'prefix', aligns to the start of the statement.
    # If set to 'prefix-indent', aligns to the start of the statement plus one
    # indentation level.
    # If set to 'child', aligns to the column of the arguments.
    dangle_align = 'prefix'

    # Specifies the minimum statement spelling length.
    # If the statement spelling length is less than this many characters, then
    # rejects nested layouts.
    min_prefix_chars = 4

    # Specifies the maximum statement spelling length.
    # If the statement spelling length is greater than the tab width by more
    # than this many characters, then rejects un-nested layouts.
    max_prefix_chars = 10

    # Specifies the maximum number of horizontally-wrapped lines allowed.
    # If a horizontally-wrapped candidate layout exceeds this many lines,
    # rejects the layout.
    max_lines_hwrap = 2
    
    # Indicates the style of line endings to use.
    line_ending = 'unix'

    # Indicates how to format command names.
    command_case = 'canonical'

    # Indicates how to format keywords.
    keyword_case = 'unchanged'

    # Provides command names that should always be wrapped.
    always_wrap = []

    # Determines whether sortable argument lists are sorted lexicographically.
    enable_sort = True

    # Determines whether the parser may infer whether an argument list is
    # sortable.
    autosort = True

    # Determines whether a valid layout is required.
    # If true, if cmake-format cannot successfully fit everything into the
    # desired line width, exits with a non-zero status code and writes nothing.
    # If false, applies the last, most aggressive attempt that it made.
    require_valid_layout = False

    # Provides a mapping of layout nodes to a list of wrap decisions.
    layout_passes = {}

# Defines options affecting comment reflow and formatting.
with section("markup"):
    # Determines whether to enable comment markup parsing and reflow.
    enable_markup = True    

    # Specifies the character to use for bulleted lists.
    bullet_char = '*'

    # Specifies the character to use as punctuation after numerals in an
    # enumeration.
    enum_char = '.'

    # Determines whether to reflow the first comment block in each file.
    first_comment_is_literal = False

    # Specifies the regex pattern to match comment blocks that shouldn't be
    # reflowed.
    literal_comment_pattern = None

    # Specifies the regex pattern to match preformat fences in comments.
    fence_pattern = '^\\s*([`~]{3}[`~]*)(.*)$'

    # Specifies the regex pattern to match rulers in comments.
    ruler_pattern = '^\\s*[^\\w\\s]{3}.*[^\\w\\s]{3}$'

    # Specifies the pattern to match the start of trailing comments for a
    # preceeding argument.
    explicit_trailing_pattern = '#<'

    # Specifies the minimum number of hashes for a hash ruler.
    hashruler_min_length = 10

    # Determines whether to canonicalize hash rulers.
    # If true, inserts a space between a first character and the remaining
    # characters of a hash ruler.
    canonicalize_hashrulers = True

# Defines options affecting the linter.
with section("lint"):
    # Provides a list of lint codes to disable.
    disabled_codes = ['C0113']

    # Specifies a regex pattern describing valid function names.
    function_pattern = '[0-9a-z_]+'

    # Specifies a regex pattern describing valid macro names.
    macro_pattern = '[0-9A-Z_]+'

    # Specifies a regex pattern describing valid names for variables with
    # global scope.
    global_var_pattern = '[A-Z][0-9A-Z_]+'

    # Specifies a regex pattern describing valid names for variables with
    # global scope, but internal semantics.
    internal_var_pattern = '_[A-Z][0-9A-Z_]+'

    # Specifies a regex pattern describing valid names for variables with local
    # scope.
    local_var_pattern = '[a-z][a-z0-9_]+'

    # Specifies a regex pattern describing valid private directory variable
    # names.
    private_var_pattern = '_[0-9a-z_]+'

    # Specifies a regex pattern describing valid public directory variable
    # names.
    public_var_pattern = '[A-Z][0-9A-Z_]+'

    # Specifies a regex pattern describing valid names for function arguments,
    # macro arguments, and loop variables.
    argument_var_pattern = '[a-z][a-z0-9_]+'

    # Specifies a regex pattern describing valid names for keywords used in
    # functions or macros.
    keyword_pattern = '[A-Z][0-9A-Z_]+'

    # Used in the heuristic for C0201.
    # Specifies how many conditionals to match within a loop before considering
    # it a parser.
    max_conditionals_custom_parser = 2

    # Specifies the minimum number of newlines allowed between statements.
    min_statement_spacing = 1

    # Specifies the maximum number of newlines allowed between statements/
    max_statement_spacing = 2

    max_returns = 6
    max_branches = 12
    max_arguments = 5
    max_localvars = 15
    max_statements = 50

# Defines options affecting file encoding.
with section("encode"):
    # Determines whether to emit a unicode byte-order mark (BOM) at the start
    # of the file.
    emit_byteorder_mark = False

    # Indicates the encoding of the input file.
    input_encoding = 'utf-8'

    # Indicates the encoding of the output file.
    output_encoding = 'utf-8'

# Defines miscellaneous options.
with section("misc"):
    # Provides configuration overrides per-command.
    per_command = {}
