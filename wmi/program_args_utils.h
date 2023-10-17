/* SPDX-FileCopyrightText: 2023 Greenbone AG
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef WMI_ARGS_H_
#define WMI_ARGS_H_

struct program_args {
    char *hostname;
    char *query;
    char *delim;
    char *ns;

};
typedef struct program_args progr_args_t;

void free_program_args(progr_args_t *);
progr_args_t *init_program_args();

#endif // finish WMI_ARGS_H_
