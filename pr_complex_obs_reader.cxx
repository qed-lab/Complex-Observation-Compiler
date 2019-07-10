#include "pr_complex_obs_reader.hxx"
#include "string_ops.hxx"
#include "PDDL.hxx"
#include <fstream>
#include <cstdlib>
#include <cctype>

#include "string_ops.h"

PR_Complex_Observation_Set_Reader::PR_Complex_Observation_Set_Reader()
{
	make_operator_index();
}

PR_Complex_Observation_Set_Reader::~PR_Complex_Observation_Set_Reader()
{
}

// Get all the operators from the domain, for easy referencing
void PR_Complex_Observation_Set_Reader::make_operator_index()
{
	PDDL::Task& task = PDDL::Task::instance();

	#ifdef DEBUG
	std::cout << "Building operator index..." << std::endl;
	#endif
  // TODO: Why 2????
	for ( unsigned op = 2; op < task.useful_ops().size(); op++ )
	{
		PDDL::Operator* op_ptr = task.useful_ops()[op];
		std::string clean_op_name = clean_string( task.str_tab().get_token( op_ptr->code() ) );
		operator_index().insert( std::make_pair( clean_op_name, op ) );
	}
}

// Trim operator name.
std::string PR_Complex_Observation_Set_Reader::clean_string( std::string& line )
{
	std::string::iterator start = line.begin();
	std::string::iterator end = line.end();

	for ( ;start != line.end(); start++)
		if ( isalnum(*start) ) break;
	for ( ;end != start; end-- )
		if ( isalnum(*end) )
		{
			end++;
			break;
		}

	std::string name( start, end );
	#ifdef DEBUG
	std::cout << "Operator found:" << name << std::endl;
	#endif
	return name;
}

std::vector<std::string> PR_Complex_Observation_Set_Reader::separate_members(std::string::iterator begin, std::string::iterator end){
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
      members.insert(member);
      itemStart = place.next();
    }
  }
  std::string member(itemStart, place);
  members.insert(member);

  return members;


}

std::vector<std::string> PR_Complex_Observation_Set_Reader::split(std::string::iterator begin, std::string::iterator end, char delim) {
    std::vector<std::string> members;

    std::string::iterator itemStart = begin;
    std::string::iterator place = begin;
    for(; place != end; ++place){
      if(*place == delim){
        std::string member(itemStart, place);
        members.insert(member);
        itemStart = place.next();
      }
    }

    return members;
}

// void PR_Complex_Observation_Set_Reader::parse( const std::string& fname )
// {
// 	std::ifstream in( fname.c_str() );
//
// 	if ( in.fail() )
// 	{
// 		std::cerr << "Could not read observations file ";
// 		std::cerr << fname << std::endl;
// 		std::cerr << "Bailing out!" << std::endl;
// 		std::exit(1);
// 	}
//
// 	char line_buffer[256000];
// 	bool op_name_parsed = false;
//
// 	std::string line;
// 	unsigned n_line = 0;
//
// 	while ( !in.eof() )
// 	{
// 		n_line++;
// 		in.getline( line_buffer, 255999, '\n' );
// 		line.assign( line_buffer );
// 		std::string op_name = clean_string( line );
// 		if ( op_name.empty() ) continue;
// 		for ( unsigned k = 0; k < op_name.size(); k++ )
// 			op_name[k] = toupper(op_name[k]);
// 		std::map< std::string, unsigned>::iterator it = operator_index().find( op_name );
// 		if ( it == operator_index().end() )
// 		{
// 			std::cout << "Could not find operator ";
// 			std::cout << "(" << op_name << ")" << std::endl;
// 			std::cout << "Bailing out!" << std::endl;
// 			std::exit(1);
// 		}
// 		Action_Execution_Complex_Observation* new_obs = new Action_Execution_Complex_Observation();
// 		new_obs->set_op_name( op_name );
// 		new_obs->set_op_index( it->second );
// 		m_set.push_back( new_obs );
// 	}
//
// 	in.close();
//
// 	std::cout << "Read from " << fname << " " << m_set.size() << " observations" << std::endl;
// 	m_set.handle_multiple_action_obs();
// }

void PR_Complex_Observation_Set_Reader::parse( const std::string& fname )
{
	std::ifstream in( fname.c_str() );

	if ( in.fail() )
	{
		std::cerr << "Could not read observations file ";
		std::cerr << fname << std::endl;
		std::cerr << "Bailing out!" << std::endl;
		std::exit(1);
	}

	char line_buffer[256000];
	bool op_name_parsed = false;

	std::string observations;
	unsigned n_line = 0;

	while ( !in.eof() )
	{
		// n_line++;
		// in.getline( line_buffer, 255999, '\n' );
		observations.assign( in );
		observations = clean_string( observations );

    std::set<std::string> no_preconditions;
    std::set<std::string> all_explanation_fluents = parse(observations, no_preconditions);


		if ( op_name.empty() ) continue;
		for ( unsigned k = 0; k < op_name.size(); k++ )
			op_name[k] = toupper(op_name[k]);
		std::map< std::string, unsigned>::iterator it = operator_index().find( op_name );
		if ( it == operator_index().end() )
		{
			std::cout << "Could not find operator ";
			std::cout << "(" << op_name << ")" << std::endl;
			std::cout << "Bailing out!" << std::endl;
			std::exit(1);
		}
		Action_Execution_Complex_Observation* new_obs = new Action_Execution_Complex_Observation();
		new_obs->set_op_name( op_name );
		new_obs->set_op_index( it->second );
		m_set.push_back( new_obs );
	}

	in.close();

	std::cout << "Read from " << fname << " " << m_set.size() << " observations" << std::endl;
	m_set.handle_multiple_action_obs();
}

std::set<std::string> PR_Complex_Observation_Set_Reader::parse(std::string observations, std::set<std::string> ordering_fluent_preconditions)
{
  std::set<std::string> contained_observation_fluents;

  observations = clean_string( observations );
  char begin_char = *(observations.begin());
  char end_char = *(observations.end().prev());

  // Case: unordered group
  if( begin_char == '{' && end_char == '}')
  {
    std::vector<std::string> group_members = separate_members(observations.begin().next(), observations.end().prev());

    // Parse every subgroup and add to the set of all observation fluents
    for (std::vector<std::string>::iterator member = group_members.begin(); member != group_members.end(); ++member) {
      std::set<std::string> member_observation_fluents = parse(*member, ordering_fluent_preconditions);
      contained_observation_fluents.insert(member_observation_fluents.begin(), member_observation_fluents.end());
    }
  }

  // Case: ordered group
  else if( begin_char == '[' && end_char == ']')
  {
    std::vector<std::string> group_members = separate_members(observations.begin().next(), observations.end().prev());

    std::set<std::string> current_observation_fluents;

    // Parse every subgroup and add to the set of all observation fluents
    for (std::vector<std::string>::iterator member = group_members.begin(); member != group_members.end(); ++member) {
      // Be sure to add the outside preconditions back in
      current_observation_fluents.insert(ordering_fluent_preconditions.begin(), ordering_fluent_preconditions.end());
      // The fluents from this group continue as preconditions to the next group
      current_observation_fluents = parse(*member, current_observation_fluents);
      contained_observation_fluents.insert(member_observation_fluents.begin(), member_observation_fluents.end());
    }
  }

  // Case: option group
  else if( begin_char == '|' && end_char == '|')
  {
    // Split by commas and parse each as a base case
    std::vector<std::string> members = split(observations.begin().next(), observations.end().prev(), ',');
    for (std::vector<std::string>::iterator member = members.begin(); member != members.end(); ++member) {

    }

  }

  // Case: a single fluent observation
  else if( begin_char == '~' && end_char == '~')
  {

  }

  // Base case: a single action observation
  else
  {
    for ( unsigned k = 0; k < observations.size(); k++ )
			observations[k] = toupper(observations[k]);
		std::map< std::string, unsigned>::iterator it = operator_index().find( observations );
		if ( it == operator_index().end() )
		{
			std::cout << "Could not find operator ";
			std::cout << "(" << observations << ")" << std::endl;
			std::cout << "Bailing out!" << std::endl;
			std::exit(1);
		}
		Action_Execution_Complex_Observation* new_obs = new Action_Execution_Complex_Observation(observations, it->second, ordering_prec_fluents, m_observation_ID_counter);
		m_set.push_back( new_obs );
    m_observation_ID_counter++;
  }

  return contained_observation_fluents;

}

std::string normalize(std::string str){
  std::string norm;
  for ( unsigned k = 0; k < str.size(); k++ )
    norm += toupper(str[k]);
}


parse_single_observation(std::string single_obs, unsigned observation_ID) {
  char begin_char = *(observations.begin());
  char end_char = *(observations.end().prev());
  if( begin_char == '~' && end_char == '~')
  {
    // Dunno what to do yet.
    std::cout << "Cannot handle fluent observations yet." << std::endl;
    exit(1);
  }
  else {
    for ( unsigned k = 0; k < observations.size(); k++ )
      observations[k] = toupper(observations[k]);
    std::map< std::string, unsigned>::iterator it = operator_index().find( observations );
    if ( it == operator_index().end() )
    {
      std::cout << "Could not find operator ";
      std::cout << "(" << observations << ")" << std::endl;
      std::cout << "Bailing out!" << std::endl;
      std::exit(1);
    }
    Action_Execution_Complex_Observation* new_obs = new Action_Execution_Complex_Observation(observations, it->second, ordering_prec_fluents, observation_ID);
    return new_obs;
  }
}
