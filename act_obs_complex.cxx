#include <cassert>
#include <cstdlib>
#include <string>
#include "act_obs_complex.hxx"
#include "pddl_string_table.hxx"
#include "PDDL.hxx"

Action_Execution_Complex_Observation::Action_Execution_Complex_Observation(std::string op_name, unsigned op_index, std::set<std::string> ordering_prec_fluents, std::string observation_ID)
	: m_ordinal( 0 ), m_observation_ID(observation_ID)
{
  set_op_name(op_name);
  set_op_index(op_index);
  set_ordering_prec_fluents(ordering_prec_fluents);
}

Action_Execution_Complex_Observation::~Action_Execution_Complex_Observation()
{
}

void Action_Execution_Complex_Observation::set_op_name( std::string& name )
{
	PDDL::Task& task = PDDL::Task::instance();

	m_str_codes.push_back( task.str_tab().get_code( name ) );
}

void Complex_Observation_Set::handle_multiple_action_obs()
{
	std::vector<unsigned> occ;

	for ( unsigned i = 0; i < size(); i++ )
	{
		if ( at(i)->ordinal() != 0 ) continue;

		occ.clear();
		occ.push_back( i );
		for ( unsigned j = i+1; j < size(); j++ )
		{
			if ( at(j)->ordinal() != 0 ) continue;
			if ( at(j)->get_op_index() == at(i)->get_op_index() )
				occ.push_back( j );
		}
		for ( unsigned j = 0; j < occ.size(); j++ )
			at( occ[j] )->set_ordinal( j+1 );
	}
}


void Complex_Observation_Set::~Complex_Observation_Set(){
  for(int i = 0; i < size(); i++)
  {
    delete at(i);
  }
}
