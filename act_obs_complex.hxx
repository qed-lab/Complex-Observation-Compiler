#ifndef __ACTION_EXECUTION_OBSERVATION_COMPLEX__
#define __ACTION_EXECUTION_OBSERVATION_COMPLEX__

#include <vector>

class Action_Execution_Complex_Observation
{
public:
	Action_Execution_Complex_Observation(std::string op_name, unsigned op_index, std::set<std::string> ordering_prec_fluents, std::string observation_ID);
	~Action_Execution_Complex_Observation();

	void 		set_op_name( std::string& name );
	void		set_op_index( unsigned index ) { m_operator = index; }
	unsigned	get_op_index() const;
	unsigned	ordinal() const { return m_ordinal; }
	void		set_ordinal( unsigned new_ord ) { m_ordinal = new_ord; }

  std::set<std::string>& ordering_prec_fluents() { return m_ordering_prec_fluents; }
  void set_ordering_prec_fluents(std::set<std::string>& ordering_preconditions) { m_ordering_prec_fluents = ordering_preconditions;}

  std::set<std::string>& observed_fluents() { return m_observed_fluents; }
  void set_observed_fluents(std::set<std::string>& observed_preconditions) { m_observed_fluents = observed_preconditions;}

  bool  is_action_observation() {return m_is_action_obs;}

private:

	std::vector<unsigned>	m_str_codes;
  std::set<std::string> m_ordering_prec_fluents;
  std::set<std::string> m_observed_fluents;
	unsigned		m_operator;
	unsigned		m_ordinal;
  bool    m_is_action_obs;
  std::string    m_observation_ID;
};

class Complex_Observation_Set : public std::vector<Action_Execution_Complex_Observation*>
{
public:
	void		handle_multiple_action_obs();
  // ~Action_Execution_Complex_Observation();

};


class Complex_Observation_Set
{
public:
  Complex_Observation_Set(std::string observation_filename);
  ~Complex_Observation_Set();

protected:
  std::set<std::string> parse(std::string observations, std::set<std::string> ordering_fluent_preconditions);
  std::vector<std::string> Complex_Observation_Set::separate_members(std::string::iterator begin, std::string::iterator end);
private:
  std::vector<Action_Execution_Complex_Observation*> m_observations;
  std::vector<std::string> m_observation_fluents;
}

inline unsigned Action_Execution_Complex_Observation::get_op_index() const
{
	return m_operator;
}


#endif // act_obs_complex.hxx
