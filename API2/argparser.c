#include "argparser.h"
#include "log.h"

struct ArgParser* argparser_ctor(
	struct ArgParser* self, 
	int argc,
	char** argv,
	struct ArgParserLongOpt* opts, 
	size_t numopts
)
{ 
	log_assert(self, "is NULL");
	log_assert(argc, "non-standard argc");
	log_assert(argv, "is NULL");
	log_assert(*argv, "is NULL");
	log_assert(opts, "is NULL");
	log_assert(numopts, "why");

	self->argv = argv;
	self->opts = opts;
	self->numopts = numopts;
	self->results = vec_ctor(struct ArgParserResult, numopts);

	for(size_t i = 0; i < numopts; i++)
	{ 
		vec_pushback(self->results, (struct ArgParserResult){0});
	}

	for(int i = 1; i < argc; i++)
	{ 
		if(argv[i][0] == '-' && argv[i][1] == '-')
		{ 
			if(argv[i][2] == '\0')
			{ 
				log_error("Expected option after '--'");
			}

			size_t arglen;
			char* argpos = strchr(argv[i], '=');
			if(argpos)
			{ 
				arglen = argpos - (argv[i] + 2);
				if(!strlen(argpos + 1))
				{ 
					log_error("No argument was given after '='");
				}
			}
			else
			{ 
				arglen = strlen(argv[i] + 2);
			}

			int found = 0;
			for(size_t j = 0; j < numopts; j++)
			{ 
				size_t len = strlen(opts[j].opt);
				if(len == arglen)
				{ 
					if(!memcmp(argv[i] + 2, opts[j].opt, len))
					{ 
						self->results[j].used = 1;
						if((opts[j].hasarg))
						{ 
							if(argpos)
							{ 
								self->results[j].arg = argpos + 1;
							}
							else
							{ 
								if(argpos)
								{ 
									log_error(
										"Option '%s' expects an argument",
										opts[j].opt
									);
								}
							}
						}
						else
						{ 
							if(argpos)
							{ 
								log_error(
									"Option '%s' does not expect an argument",
									opts[j].opt
								);
							}
						}
						found = 1;
						break;
					}
				}
			}

			if(!found)
			{ 
				log_error("There is no option called '%s'", argv[i] + 2);
			}
		}
		else
		{ 
			log_error("Expected '--' before option");
		}
	}

	return self;
}

void argparser_printhelp(struct ArgParser* self)
{ 
	log_assert(self, "is NULL");

	printf("Usage: %s [options] [--option=arg]\nOptions:\n", self->argv[0]);
	for(size_t i = 0; i < self->numopts; i++)
	{
		printf("    --%s", self->opts[i].opt);

		if(self->opts[i].info)
		{
			int gap = 15 - strlen(self->opts[i].opt);
			for(int j = 0; j < gap; j++)
			{
				putchar(' ');
			}

			puts(self->opts[i].info);
		}
		else
		{
			putchar('\n');
		}
	}

	putchar('\n');
}

void argparser_dtor(struct ArgParser* self)
{ 
	log_assert(self, "is NULL");
	vec_dtor(self->results);
}

