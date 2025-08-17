/*
 * src/KnowledgeIntegrator.cpp
 *
 * Copyright (C) 2024 OpenCog Foundation
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Knowledge Integrator Implementation
 * Bridges knowledge representation with AtomSpace operations
 * Part of the AGENT-ZERO-GENESIS project
 */

#include <sstream>
#include <algorithm>

#include <opencog/atoms/atom_types/types.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/truthvalue/SimpleTruthValue.h>

#include "opencog/agentzero/KnowledgeIntegrator.h"
#include "opencog/agentzero/AgentZeroCore.h"

using namespace opencog;
using namespace opencog::agentzero;

KnowledgeIntegrator::KnowledgeIntegrator(AgentZeroCore* agent_core, AtomSpacePtr atomspace)
    : _agent_core(agent_core)
    , _atomspace(atomspace)
    , _knowledge_base(Handle::UNDEFINED)
    , _working_knowledge(Handle::UNDEFINED)
    , _semantic_network(Handle::UNDEFINED)
    , _episodic_memory(Handle::UNDEFINED)
    , _procedural_memory(Handle::UNDEFINED)
    , _enable_concept_formation(true)
    , _enable_semantic_integration(true)
    , _enable_memory_consolidation(true)
    , _knowledge_threshold(0.5)
{
    logger().info() << "[KnowledgeIntegrator] Constructor: Initializing knowledge integration";
    initializeKnowledgeStructures();
}

KnowledgeIntegrator::~KnowledgeIntegrator()
{
    logger().info() << "[KnowledgeIntegrator] Destructor: Cleaning up knowledge integration";
}

Handle KnowledgeIntegrator::addFact(const std::string& fact_description, ConfidenceLevel confidence)
{
    logger().debug() << "[KnowledgeIntegrator] Adding fact: " << fact_description;
    
    Handle fact_atom = createKnowledgeAtom(fact_description, KnowledgeType::FACTUAL);
    
    // Set truth value based on confidence
    double confidence_value = static_cast<double>(confidence) / 100.0;
    TruthValuePtr fact_tv = SimpleTruthValue::createTV(confidence_value, 0.9);
    fact_atom->setTruthValue(fact_tv);
    
    // Link to knowledge base
    HandleSeq fact_link;
    fact_link.push_back(_knowledge_base);
    fact_link.push_back(fact_atom);
    _atomspace->add_link(MEMBER_LINK, std::move(fact_link));
    
    return fact_atom;
}

Handle KnowledgeIntegrator::addProcedure(const std::string& procedure_description,
                                        const std::vector<std::string>& steps,
                                        ConfidenceLevel confidence)
{
    logger().debug() << "[KnowledgeIntegrator] Adding procedure: " << procedure_description;
    
    Handle procedure_atom = createKnowledgeAtom(procedure_description, KnowledgeType::PROCEDURAL);
    
    // Create step atoms and link them
    for (size_t i = 0; i < steps.size(); ++i) {
        Handle step_atom = _atomspace->add_node(CONCEPT_NODE, "Step_" + std::to_string(i) + "_" + steps[i]);
        
        HandleSeq step_link;
        step_link.push_back(procedure_atom);
        step_link.push_back(step_atom);
        _atomspace->add_link(SEQUENTIAL_AND_LINK, std::move(step_link));
    }
    
    // Set confidence
    double confidence_value = static_cast<double>(confidence) / 100.0;
    TruthValuePtr proc_tv = SimpleTruthValue::createTV(confidence_value, 0.9);
    procedure_atom->setTruthValue(proc_tv);
    
    return procedure_atom;
}

Handle KnowledgeIntegrator::addEpisode(const std::string& experience_description,
                                      const std::vector<Handle>& context_atoms,
                                      ConfidenceLevel confidence)
{
    logger().debug() << "[KnowledgeIntegrator] Adding episode: " << experience_description;
    
    Handle episode_atom = createKnowledgeAtom(experience_description, KnowledgeType::EPISODIC);
    
    // Link context atoms
    for (const Handle& context : context_atoms) {
        HandleSeq context_link;
        context_link.push_back(episode_atom);
        context_link.push_back(context);
        _atomspace->add_link(EVALUATION_LINK, std::move(context_link));
    }
    
    // Set confidence
    double confidence_value = static_cast<double>(confidence) / 100.0;
    TruthValuePtr episode_tv = SimpleTruthValue::createTV(confidence_value, 0.9);
    episode_atom->setTruthValue(episode_tv);
    
    return episode_atom;
}

Handle KnowledgeIntegrator::addSemanticRelation(const std::string& concept1_name,
                                               const std::string& relation_type,
                                               const std::string& concept2_name,
                                               ConfidenceLevel confidence)
{
    logger().debug() << "[KnowledgeIntegrator] Adding semantic relation: " 
                    << concept1_name << " " << relation_type << " " << concept2_name;
    
    Handle concept1 = findOrCreateConcept(concept1_name);
    Handle concept2 = findOrCreateConcept(concept2_name);
    
    establishConceptRelation(concept1, concept2, relation_type);
    
    // Create relation atom
    Handle relation_atom = _atomspace->add_node(CONCEPT_NODE, relation_type + "_" + concept1_name + "_" + concept2_name);
    
    // Set confidence
    double confidence_value = static_cast<double>(confidence) / 100.0;
    TruthValuePtr rel_tv = SimpleTruthValue::createTV(confidence_value, 0.9);
    relation_atom->setTruthValue(rel_tv);
    
    return relation_atom;
}

std::vector<Handle> KnowledgeIntegrator::queryKnowledge(const std::string& query_text, int max_results)
{
    logger().debug() << "[KnowledgeIntegrator] Querying knowledge: " << query_text;
    
    std::vector<Handle> results;
    
    // Simplified query implementation - search for atoms containing query terms
    HandleSeq all_atoms;
    _atomspace->get_handles_by_type(all_atoms, ATOM, true);
    
    for (const Handle& atom : all_atoms) {
        std::string atom_name = atom->get_name();
        if (atom_name.find(query_text) != std::string::npos) {
            results.push_back(atom);
            if (results.size() >= static_cast<size_t>(max_results)) {
                break;
            }
        }
    }
    
    return results;
}

std::vector<Handle> KnowledgeIntegrator::getFactsAbout(const std::string& concept_name)
{
    logger().debug() << "[KnowledgeIntegrator] Getting facts about: " << concept_name;
    
    std::vector<Handle> facts;
    
    // Find concept atom
    Handle concept = findOrCreateConcept(concept_name);
    
    // Find related knowledge atoms
    facts = findRelatedKnowledge(concept);
    
    return facts;
}

std::vector<Handle> KnowledgeIntegrator::getProceduresFor(const std::string& task_description)
{
    logger().debug() << "[KnowledgeIntegrator] Getting procedures for: " << task_description;
    
    // Simplified procedure search
    return queryKnowledge(task_description, 5);
}

std::vector<Handle> KnowledgeIntegrator::getEpisodesRelatedTo(const std::vector<Handle>& context_atoms)
{
    logger().debug() << "[KnowledgeIntegrator] Getting episodes related to context";
    
    std::vector<Handle> episodes;
    
    // Find episodes that share context atoms
    for (const Handle& context : context_atoms) {
        auto related = findRelatedKnowledge(context);
        episodes.insert(episodes.end(), related.begin(), related.end());
    }
    
    // Remove duplicates
    std::sort(episodes.begin(), episodes.end());
    episodes.erase(std::unique(episodes.begin(), episodes.end()), episodes.end());
    
    return episodes;
}

std::vector<Handle> KnowledgeIntegrator::getSemanticRelations(const std::string& concept_name,
                                                             const std::string& relation_type)
{
    logger().debug() << "[KnowledgeIntegrator] Getting semantic relations for: " << concept_name;
    
    Handle concept = findOrCreateConcept(concept_name);
    return findRelatedKnowledge(concept);
}

Handle KnowledgeIntegrator::registerConcept(const std::string& concept_name, 
                                           const std::string& concept_description)
{
    logger().debug() << "[KnowledgeIntegrator] Registering concept: " << concept_name;
    
    Handle concept = findOrCreateConcept(concept_name);
    
    if (!concept_description.empty()) {
        // Add description as knowledge
        addFact(concept_name + " is " + concept_description, ConfidenceLevel::HIGH);
    }
    
    return concept;
}

bool KnowledgeIntegrator::hasKnowledgeAbout(const std::string& concept_name)
{
    auto it = _concept_registry.find(concept_name);
    return it != _concept_registry.end();
}

std::vector<Handle> KnowledgeIntegrator::getAllConcepts()
{
    std::vector<Handle> concepts;
    
    for (const auto& pair : _concept_registry) {
        concepts.push_back(pair.second);
    }
    
    return concepts;
}

std::map<std::string, int> KnowledgeIntegrator::getKnowledgeStatistics()
{
    std::map<std::string, int> stats;
    
    stats["total_concepts"] = _concept_registry.size();
    stats["active_knowledge"] = _active_knowledge.size();
    stats["total_atoms"] = _atomspace->get_size();
    
    return stats;
}

std::string KnowledgeIntegrator::getStatusInfo() const
{
    std::ostringstream status;
    status << "{";
    status << "\"total_concepts\":" << _concept_registry.size() << ",";
    status << "\"active_knowledge\":" << _active_knowledge.size() << ",";
    status << "\"concept_formation_enabled\":" << (_enable_concept_formation ? "true" : "false") << ",";
    status << "\"semantic_integration_enabled\":" << (_enable_semantic_integration ? "true" : "false") << ",";
    status << "\"memory_consolidation_enabled\":" << (_enable_memory_consolidation ? "true" : "false") << ",";
    status << "\"knowledge_threshold\":" << _knowledge_threshold;
    status << "}";
    return status.str();
}

bool KnowledgeIntegrator::processKnowledgeIntegration()
{
    logger().debug() << "[KnowledgeIntegrator] Processing knowledge integration cycle";
    
    try {
        // Perform memory consolidation if enabled
        if (_enable_memory_consolidation) {
            consolidateMemory();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error in knowledge processing: " << e.what();
        return false;
    }
}

// Private implementation methods

void KnowledgeIntegrator::initializeKnowledgeStructures()
{
    logger().debug() << "[KnowledgeIntegrator] Initializing knowledge structures";
    
    std::string agent_name = _agent_core->getAgentName();
    _knowledge_base = _atomspace->add_node(CONCEPT_NODE, agent_name + "_KnowledgeBase");
    _working_knowledge = _atomspace->add_node(CONCEPT_NODE, agent_name + "_WorkingKnowledge");
    _semantic_network = _atomspace->add_node(CONCEPT_NODE, agent_name + "_SemanticNetwork");
    _episodic_memory = _atomspace->add_node(CONCEPT_NODE, agent_name + "_EpisodicMemory");
    _procedural_memory = _atomspace->add_node(CONCEPT_NODE, agent_name + "_ProceduralMemory");
    
    // Initialize knowledge categories
    _knowledge_categories[KnowledgeType::FACTUAL] = _atomspace->add_node(CONCEPT_NODE, agent_name + "_Facts");
    _knowledge_categories[KnowledgeType::PROCEDURAL] = _procedural_memory;
    _knowledge_categories[KnowledgeType::EPISODIC] = _episodic_memory;
    _knowledge_categories[KnowledgeType::SEMANTIC] = _semantic_network;
}

Handle KnowledgeIntegrator::createKnowledgeAtom(const std::string& content, KnowledgeType type)
{
    std::string prefix;
    switch (type) {
        case KnowledgeType::FACTUAL: prefix = "Fact_"; break;
        case KnowledgeType::PROCEDURAL: prefix = "Proc_"; break;
        case KnowledgeType::EPISODIC: prefix = "Episode_"; break;
        case KnowledgeType::SEMANTIC: prefix = "Semantic_"; break;
        case KnowledgeType::CONDITIONAL: prefix = "Rule_"; break;
        case KnowledgeType::TEMPORAL: prefix = "Temporal_"; break;
    }
    
    Handle knowledge_atom = _atomspace->add_node(CONCEPT_NODE, prefix + content);
    
    // Link to appropriate category
    auto cat_it = _knowledge_categories.find(type);
    if (cat_it != _knowledge_categories.end()) {
        HandleSeq cat_link;
        cat_link.push_back(cat_it->second);
        cat_link.push_back(knowledge_atom);
        _atomspace->add_link(MEMBER_LINK, std::move(cat_link));
    }
    
    return knowledge_atom;
}

Handle KnowledgeIntegrator::findOrCreateConcept(const std::string& concept_name)
{
    auto it = _concept_registry.find(concept_name);
    if (it != _concept_registry.end()) {
        return it->second;
    }
    
    // Create new concept
    Handle concept = _atomspace->add_node(CONCEPT_NODE, std::string(concept_name));
    _concept_registry[concept_name] = concept;
    
    return concept;
}

void KnowledgeIntegrator::establishConceptRelation(const Handle& concept1, const Handle& concept2, 
                                                  const std::string& relation_type)
{
    // Create appropriate link type based on relation
    Type link_type = EVALUATION_LINK;
    if (relation_type == "isa") {
        link_type = INHERITANCE_LINK;
    } else if (relation_type == "has") {
        link_type = MEMBER_LINK;
    }
    
    HandleSeq relation_link;
    relation_link.push_back(concept1);
    relation_link.push_back(concept2);
    _atomspace->add_link(link_type, std::move(relation_link));
}

std::vector<Handle> KnowledgeIntegrator::findRelatedKnowledge(const Handle& query_atom)
{
    std::vector<Handle> related;
    
    // Find incoming links to the query atom
    const IncomingSet& incoming = query_atom->getIncomingSet();
    
    for (const Handle& link : incoming) {
        const HandleSeq& outgoing = link->getOutgoingSet();
        for (const Handle& atom : outgoing) {
            if (atom != query_atom) {
                related.push_back(atom);
            }
        }
    }
    
    return related;
}

void KnowledgeIntegrator::consolidateMemory()
{
    logger().debug() << "[KnowledgeIntegrator] Consolidating memory";
    
    // Simplified memory consolidation - in a real implementation this would:
    // - Identify frequently accessed knowledge
    // - Strengthen important connections
    // - Weaken or remove unused knowledge
    // - Form higher-level abstractions
}