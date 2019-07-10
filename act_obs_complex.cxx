#include <cassert>
#include <cstdlib>
#include <string>
#include <iostream>
#include "act_obs_complex.hxx"
#include "PDDL.hxx"

#include "string_ops.hxx"

Action_Execution_Complex_Observation::Action_Execution_Complex_Observation(std::string op_name, unsigned op_index, std::set<std::string> ordering_prec_fluents, std::string observation_ID)
	: m_ordinal( 0 ), m_observation_ID(observation_ID), m_is_action_obs(true)
{
  set_op_name(op_name);
  set_op_index(op_index);
  set_ordering_prec_fluents(ordering_prec_fluents);
}

Action_Execution_Complex_Observation::Action_Execution_Complex_Observation(std::set<std::string> observed_fluents, std::set<std::string> ordering_prec_fluents, std::string observation_ID)
	: m_ordinal( 0 ), m_observation_ID(observation_ID), m_is_action_obs(false)
{
  set_observed_fluents(observed_fluents);
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



void Action_Execution_Complex_Observation::print(std::ostream& os)
{
	os << m_observation_ID << ":"; // << m_observation_string;
  if (m_is_action_obs) {
    os << "\tobserved action #: "<< m_operator;
  } else {
    os << "\tobserved fluents:\n";
    for (std::set<std::string>::iterator fl = m_observed_fluents.begin(); fl != m_observed_fluents.end(); ++fl) {
      os << "\n\t\t" << *fl;
    }
  }
  os << "\n\tpreconditions:\n";
  for (std::set<std::string>::iterator fl = m_ordering_prec_fluents.begin(); fl != m_ordering_prec_fluents.end(); ++fl) {
    os << "\t\t" << *fl << "\n";
  }
  os << std::endl;
}





// void Complex_Observation_Set::handle_multiple_action_obs()
// {
// 	std::vector<unsigned> occ;
//
// 	for ( unsigned i = 0; i < size(); i++ )
// 	{
// 		if ( at(i)->ordinal() != 0 ) continue;
//
// 		occ.clear();
// 		occ.push_back( i );
// 		for ( unsigned j = i+1; j < size(); j++ )
// 		{
// 			if ( at(j)->ordinal() != 0 ) continue;
// 			if ( at(j)->get_op_index() == at(i)->get_op_index() )
// 				occ.push_back( j );
// 		}
// 		for ( unsigned j = 0; j < occ.size(); j++ )
// 			at( occ[j] )->set_ordinal( j+1 );
// 	}
// }


Complex_Observation_Set::Complex_Observation_Set(std::string observation_filename)
: m_observation_ID_counter(0), m_observations()
{

  std::ifstream in( observation_filename.c_str() );

	if ( in.fail() )
	{
		std::cerr << "Could not read observations file ";
		std::cerr << observation_filename << std::endl;
		std::cerr << "Bailing out!" << std::endl;
		std::exit(1);
	}

  // Get the operators from the domain, for easy referencing
  make_operator_index();

	// std::string observations;
	unsigned n_line = 0;

  // observations.assign( in );
  std::stringstream buffer;
  buffer << in.rdbuf();
  std::string observations = buffer.str();
  observations = strip( observations );

  std::cout << observations << std::endl;

  std::set<std::string> no_preconditions;
  m_observation_fluents = parse(observations, no_preconditions);

  in.close();

	std::cout << "Read from " << observation_filename << " " << m_observation_fluents.size() << " observations" << std::endl;
	// m_set.handle_multiple_action_obs();

}

Complex_Observation_Set::~Complex_Observation_Set()
{
  // Clean up the allocated memory for observations
  for (int i = 0; i < m_observations.size(); i++) {
    delete m_observations[i];
  }
}


void Complex_Observation_Set::print_all(std::ostream& os){
  os << "Observations: \n";
  for (int i = 0; i < m_observations.size(); i++) {
    m_observations[i]->print(os);
  }
  os <<std::endl;
}


// Get all the operators from the domain, for easy referencing
void Complex_Observation_Set::make_operator_index()
{
	PDDL::Task& task = PDDL::Task::instance();

	#ifdef DEBUG
	std::cout << "Building operator index..." << std::endl;
	#endif
  // TODO: Why 2????
	for ( unsigned op = 2; op < task.useful_ops().size(); op++ )
	{
		PDDL::Operator* op_ptr = task.useful_ops()[op];
		std::string clean_op_name = strip( task.str_tab().get_token( op_ptr->code() ) );
		operator_index().insert( std::make_pair( clean_op_name, op ) );
	}
}

std::set<std::string> Complex_Observation_Set::parse(std::string observations, std::set<std::string> ordering_fluent_preconditions)
{
  std::set<std::string> contained_observation_fluents;

  observations = strip( observations );
  std::cout << observations << std::endl;
  std::string::iterator obs_start = observations.begin();
  std::string::iterator obs_end = std::prev(observations.end());
  char begin_char = *(obs_start);
  char end_char = *(obs_end);

  // Case: unordered group
  if( begin_char == '{' && end_char == '}')
  {
    std::vector<std::string> group_members = separate_members(std::next(obs_start), std::prev(obs_end));

    // Parse every subgroup and add to the set of all observation fluents
    for (std::vector<std::string>::iterator member = group_members.begin(); member != group_members.end(); ++member) {
      std::set<std::string> member_observation_fluents = parse(*member, ordering_fluent_preconditions);
      contained_observation_fluents.insert(member_observation_fluents.begin(), member_observation_fluents.end());
    }
  }

  // Case: ordered group
  else if( begin_char == '[' && end_char == ']')
  {
    std::vector<std::string> group_members = separate_members(std::next(obs_start), std::prev(obs_end));

    std::set<std::string> current_observation_fluents;

    // Parse every subgroup and add to the set of all observation fluents
    for (std::vector<std::string>::iterator member = group_members.begin(); member != group_members.end(); ++member) {
      // Be sure to add the outside preconditions back in
      current_observation_fluents.insert(ordering_fluent_preconditions.begin(), ordering_fluent_preconditions.end());
      // The fluents from this group continue as preconditions to the next group
      current_observation_fluents = parse(*member, current_observation_fluents);
      contained_observation_fluents.insert(current_observation_fluents.begin(), current_observation_fluents.end());
    }
  }

  // Case: option group
  else if( begin_char == '|' && end_char == '|')
  {
    // Split by commas and parse each as a base case
    std::vector<std::string> members = split(std::next(obs_start), std::prev(obs_end), ',');
    std::string observed_fluent = "OBSERVED_OB_ID_" + std::to_string(m_observation_ID_counter);
    m_observation_ID_counter++;
    for (std::vector<std::string>::iterator member = members.begin(); member != members.end(); ++member) {
      observed_fluent = add_observation(strip(*member), true, observed_fluent, ordering_fluent_preconditions);
      contained_observation_fluents.insert(observed_fluent);
    }

  }

  // Case: a single fluent observation
  else if( begin_char == '~' && end_char == '~')
  {

  }

  // Base case: a single action observation
  else
  {
    std::string observed_fluent = "OBSERVED_OB_ID_" + std::to_string(m_observation_ID_counter);
    m_observation_ID_counter++;
    observed_fluent = add_observation(observations, true, observed_fluent, ordering_fluent_preconditions);
    contained_observation_fluents.insert(observed_fluent);

  }

  return contained_observation_fluents;

}


std::string Complex_Observation_Set::add_observation(std::string observation, bool is_action_observation, std::string observation_ID, std::set<std::string> ordering_fluents){

  if( is_action_observation){
    for ( unsigned k = 0; k < observation.size(); k++ )
			observation[k] = toupper(observation[k]);
		std::map< std::string, unsigned>::iterator it = operator_index().find( observation );
		if ( it == operator_index().end() )
		{
			std::cout << "Could not find operator ";
			std::cout << "(" << observation << ")" << std::endl;
			std::cout << "Bailing out!" << std::endl;
			std::exit(1);
		}
		Action_Execution_Complex_Observation* new_obs = new Action_Execution_Complex_Observation(observation, it->second, ordering_fluents, observation_ID);
		m_observations.push_back( new_obs );
    return new_obs->observed_fluent();
  }

}

std::vector<std::string> Complex_Observation_Set::separate_members(std::string::iterator begin, std::string::iterator end){
  std::vector<std::string> members;

  int braces = 0; // How many enclosing braces we're inside
  int brackets = 0; // How many enclosing brackets we're inside
  char bar = 0; // In an option group or not

  std::string::iterator itemStart = begin;
  std::string::iterator place = begin;
  for(; place != end; ++place) {
    char the_char = *place;
    if(the_char == '{') {
      braces++;
      continue;
    }
    if(the_char == '}') {
      braces--;
      continue;
    }
    if(the_char == '[') {
      brackets++;
      continue;
    }
    if(the_char == ']') {
      brackets--;
      continue;
    }
    if( the_char == '|') {
      bar = !bar;
      continue;
    }
    if(the_char == ',' && braces == 0 && brackets == 0 && !bar) {
      std::string member(itemStart, place);
      members.push_back(member);
      itemStart = std::next(place);
    }
  }
  std::string member(itemStart, place);
  members.push_back(member);

  return members;


}


std::vector<std::string> Complex_Observation_Set::split(std::string::iterator begin, std::string::iterator end, char delim) {
    std::vector<std::string> members;

    std::string::iterator itemStart = begin;
    std::string::iterator place = begin;
    for(; place != end; ++place){
      if(*place == delim){
        std::string member(itemStart, place);
        members.push_back(member);
        itemStart = std::next(place);
      }
    }
    std::string member(itemStart, std::next(place));
    members.push_back(member);

    return members;
}
