/*-------------------------------------------------------------------------
 *
 * planmain.h
 *	  prototypes for various files in optimizer/plan
 *
 *
 * Portions Copyright (c) 2012-2014, TransLattice, Inc.
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/planmain.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PLANMAIN_H
#define PLANMAIN_H

#include "nodes/plannodes.h"
#include "nodes/relation.h"
#ifdef XCP
#include "pgxc/planner.h"
#endif

/* possible values for force_parallel_mode */
typedef enum
{
	FORCE_PARALLEL_OFF,
	FORCE_PARALLEL_ON,
	FORCE_PARALLEL_REGRESS
}	ForceParallelMode;

/* GUC parameters */
#define DEFAULT_CURSOR_TUPLE_FRACTION 0.1
extern double cursor_tuple_fraction;
extern int	force_parallel_mode;

/* query_planner callback to compute query_pathkeys */
typedef void (*query_pathkeys_callback) (PlannerInfo *root, void *extra);

/*
 * prototypes for plan/planmain.c
 */
extern RelOptInfo *query_planner(PlannerInfo *root, List *tlist,
			  query_pathkeys_callback qp_callback, void *qp_extra);

/*
 * prototypes for plan/planagg.c
 */
extern void preprocess_minmax_aggregates(PlannerInfo *root, List *tlist);

/*
 * prototypes for plan/createplan.c
 */
extern Plan *create_plan(PlannerInfo *root, Path *best_path);
extern ForeignScan *make_foreignscan(List *qptlist, List *qpqual,
				 Index scanrelid, List *fdw_exprs, List *fdw_private,
				 List *fdw_scan_tlist, List *fdw_recheck_quals,
				 Plan *outer_plan);
extern Plan *materialize_finished_plan(Plan *subplan);
extern bool is_projection_capable_path(Path *path);
extern bool is_projection_capable_plan(Plan *plan);

/* External use of these functions is deprecated: */
extern Sort *make_sort_from_sortclauses(List *sortcls, Plan *lefttree);
extern Agg *make_agg(List *tlist, List *qual,
		 AggStrategy aggstrategy, AggSplit aggsplit,
		 int numGroupCols, AttrNumber *grpColIdx, Oid *grpOperators,
		 List *groupingSets, List *chain,
		 double dNumGroups, Plan *lefttree);
extern Limit *make_limit(Plan *lefttree, Node *limitOffset, Node *limitCount);

/*
 * prototypes for plan/initsplan.c
 */
extern int	from_collapse_limit;
extern int	join_collapse_limit;

extern void add_base_rels_to_query(PlannerInfo *root, Node *jtnode);
extern void build_base_rel_tlists(PlannerInfo *root, List *final_tlist);
extern void add_vars_to_targetlist(PlannerInfo *root, List *vars,
					   Relids where_needed, bool create_new_ph);
extern void find_lateral_references(PlannerInfo *root);
extern void create_lateral_join_info(PlannerInfo *root);
extern List *deconstruct_jointree(PlannerInfo *root);
extern void distribute_restrictinfo_to_rels(PlannerInfo *root,
								RestrictInfo *restrictinfo);
extern void process_implied_equality(PlannerInfo *root,
						 Oid opno,
						 Oid collation,
						 Expr *item1,
						 Expr *item2,
						 Relids qualscope,
						 Relids nullable_relids,
						 bool below_outer_join,
						 bool both_const);
extern RestrictInfo *build_implied_join_equality(Oid opno,
							Oid collation,
							Expr *item1,
							Expr *item2,
							Relids qualscope,
							Relids nullable_relids);
extern void match_foreign_keys_to_quals(PlannerInfo *root);

/*
 * prototypes for plan/analyzejoins.c
 */
extern List *remove_useless_joins(PlannerInfo *root, List *joinlist);
extern bool query_supports_distinctness(Query *query);
extern bool query_is_distinct_for(Query *query, List *colnos, List *opids);

/*
 * prototypes for plan/setrefs.c
 */
extern Plan *set_plan_references(PlannerInfo *root, Plan *plan);
extern void record_plan_function_dependency(PlannerInfo *root, Oid funcid);
extern void extract_query_dependencies(Node *query,
						   List **relationOids,
						   List **invalItems,
						   bool *hasRowSecurity);

#ifdef PGXC
extern RemoteSubplan *find_push_down_plan(Plan *plan, bool force);
extern RemoteSubplan *find_delete_push_down_plan(PlannerInfo *root, Plan *plan,
		bool force, Plan **parent);
extern RemoteSubplan *make_remotesubplan(PlannerInfo *root,
				   Plan *lefttree,
				   Distribution *resultDistribution,
				   Distribution *execDistribution,
				   List *pathkeys);
#endif /* PGXC */

#endif   /* PLANMAIN_H */
