#pragma once
/*
 * Meta-characters:
 *   \: escape character
 *   .: any character
 *   *: [closure] repeats 0 or more times
 *   +: repeats 1 or more times
 *   ?: repeats 0 or 1 time
 *   |: [union] or
 *   (: group start
 *   ): group end
 *   [: class start
 *   ]: class end
 *
 * regex -> regex "|" term | empty
 * term -> term factor | factor
 * factor -> factor repeat | reusable
 * repeat -> "*" | "+" | "?"
 *
 * reusable -> group | unit
 * group -> "(" regex ")"
 * unit -> class | escaped_unit
 * [prior] class -> "[^" class_escaped_units "]"
 * class -> "[" class_escaped_units "]"
 * class_escaped_units -> class_escaped_units class_escaped_unit | empty
 *
 * escaped_unit:
 *   single character other than "\*+?|()[]", including "."
 *   backslash followed by "\.*+?|()[]abefnrtv"
 *   backslash followed by 1--3 octet digits
 *   user-defined single-alpha escaping sequence
 *   user-defined sub-expression referenced by "\u<ID>" like "\u<integer>"
 *
 * class_escaped_unit:
 *   alpha/digit range like "b-f"
 *   single character other than "\-[]"
 *   backslash followed by "\-[]abefnrtv" or 1--3 octet digits
 *   user-defined sequence
 *
 * Other than explicitly specified, '\0' is not considered as a part of
 * accepting characters for any pattern.
 */
typedef struct regex_t regex_t;
typedef struct regex_config_t regex_config_t;
typedef struct charset_t charset_t;

#ifdef ENABLE_VISIBILITY
#pragma GCC visibility push(default)
#endif  /* ENABLE_VISIBILITY */

regex_t *regex_create(const regex_config_t *, const char *);
void regex_destroy(regex_t *);
regex_config_t *regex_config_create(void);
void regex_config_destroy(regex_config_t *);

int regex_compile(regex_t *, int flags);
void regex_print_compiled_expr(const regex_t *);
void regex_print_poses(const regex_t *);
void regex_print_char_class(const regex_t *);

void regex_config_add_escaping(regex_config_t *, unsigned char, const charset_t *);
void regex_config_add_escaping_ch(regex_config_t *, unsigned char, unsigned char);
int regex_config_add_escaping_str(regex_config_t *, unsigned char, const char *);
void regex_config_add_subex(regex_config_t *, const char *id, regex_t *);
int regex_config_add_subex_str(regex_config_t *, const char *id, const char *);

#ifdef ENABLE_VISIBILITY
#pragma GCC visibility pop
#endif  /* ENABLE_VISIBILITY */
