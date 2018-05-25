const char *argp_program_version =
"scent 1.0";

const char *argp_program_bug_address =
"<benedikt.stufler@posteo.net>";


/* This structure is used by main to communicate with parse_opt. */
struct cmdarg
{
    int threads;
    int start;
    int end;
    char *infile;
    char *outfile;
};



/*
     OPTIONS.   Field 1 in ARGP.
     Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
    {"threads", 't', "THREADS", 0, "Distribute the workload on THREADS many threads"},
    {"start",   's', "START", 0, "Restrict to vertices with index at least START"},
    {"end",     'e', "END", 0, "Restrict to vertices with index less than END"},    {"infile",  'i', "INFILE", 0, "Read graphml input from INFILE instead of standard input"},
    {"outfile", 'o', "OUTFILE", 0, "Output to OUTFILE instead of to standard output"},
    {0}
};


/*
     PARSER. Field 2 in ARGP.
     Order of parameters: KEY, ARG, STATE.
*/
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct cmdarg *arguments = state->input;

    switch (key) {
        case 't':
            arguments->threads = (int) strtoimax(arg, NULL, 10);
            break;
        case 'i':
            arguments->infile = arg;
            break;
        case 'o':
            arguments->outfile = arg;
            break;
        case 's':
            arguments->start = (int) strtoimax(arg, NULL, 10);
            break;
        case 'e':
            arguments->end = (int) strtoimax(arg, NULL, 10);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


/*
     ARGS_DOC. Field 3 in ARGP.
     A description of the non-option command-line arguments
         that we accept.
*/
static char args_doc[] = "";

/*
    DOC.    Field 4 in ARGP.
    Program documentation.
*/
static char doc[] = "scent -- A simple approach to calculating the closeness centrality of vertices in a graph.";

/*
     The ARGP structure itself.
*/
static struct argp argp = {options, parse_opt, args_doc, doc};



int getcmdargs(struct cmdarg *comarg, int argc, char **argv) {

	/* set command line arguments defaults */
    comarg->threads = 1;
    comarg->outfile = NULL;
    comarg->infile = NULL;
    comarg->start = -1;
    comarg->end = -1;


	/* read command line arguments */
    argp_parse (&argp, argc, argv, 0, 0, comarg);

	return 0;
}

