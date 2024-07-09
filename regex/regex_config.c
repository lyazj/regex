#include "regex_impl.h"
#include "util.h"
#include "charset.h"

regex_config_t *regex_config_create(void)
{
  regex_config_t *config = (regex_config_t *)Malloc(sizeof *config);
  for(unsigned i = 0; i < 256; ++i) {
    regex_config_add_escaping_ch(config, i, i);
  }
  regex_config_add_escaping_ch(config, 'a', '\a');
  regex_config_add_escaping_ch(config, 'b', '\b');
  regex_config_add_escaping_ch(config, 'f', '\f');
  regex_config_add_escaping_ch(config, 'n', '\n');
  regex_config_add_escaping_ch(config, 'r', '\r');
  regex_config_add_escaping_ch(config, 't', '\t');
  regex_config_add_escaping_ch(config, 'v', '\v');
  return config;
}

void regex_config_destroy(regex_config_t *config)
{
  if(config == NULL) return;
  Free(config);
}

void regex_config_add_escaping(regex_config_t *config, unsigned char uc, const charset_t *sp)
{
  config->escape[uc] = *sp;
}

void regex_config_add_escaping_ch(regex_config_t *config, unsigned char uc, unsigned char uc2)
{
  charset_t s;
  charset_empty(&s);
  charset_set(&s, uc2);
  regex_config_add_escaping(config, uc, &s);
}

int regex_config_add_escaping_str(regex_config_t *config, unsigned char uc, const char *str)
{
  charset_t s;
  regex_t *regex = regex_create(config, str);

  if(!parse_regex_unit(regex, &s)) {
    regex_destroy(regex);
    return 0;
  }
  regex_destroy(regex);

  regex_config_add_escaping(config, uc, &s);
  return 1;
}

void regex_config_add_subex(regex_config_t *, const char *id, regex_t *);
int regex_config_add_subex_str(regex_config_t *, const char *id, const char *);
