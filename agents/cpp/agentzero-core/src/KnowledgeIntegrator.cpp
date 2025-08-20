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
#include <cctype>

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
    _knowledge_base = _atomspace->add_node(CONCEPT_NODE, std::string(agent_name + "_KnowledgeBase"));
    _working_knowledge = _atomspace->add_node(CONCEPT_NODE, std::string(agent_name + "_WorkingKnowledge"));
    _semantic_network = _atomspace->add_node(CONCEPT_NODE, std::string(agent_name + "_SemanticNetwork"));
    _episodic_memory = _atomspace->add_node(CONCEPT_NODE, std::string(agent_name + "_EpisodicMemory"));
    _procedural_memory = _atomspace->add_node(CONCEPT_NODE, std::string(agent_name + "_ProceduralMemory"));
    
    // Initialize knowledge categories
    _knowledge_categories[KnowledgeType::FACTUAL] = _atomspace->add_node(CONCEPT_NODE, std::string(agent_name + "_Facts"));
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
    
    Handle knowledge_atom = _atomspace->add_node(CONCEPT_NODE, std::string(prefix + content));
    
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
    Handle concept = _atomspace->add_node(CONCEPT_NODE, std::string(std::string(concept_name)));
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

std::vector<Handle> KnowledgeIntegrator::formConceptsFrom(const std::vector<Handle>& experience_atoms)
{
    logger().debug() << "[KnowledgeIntegrator] Forming concepts from " << experience_atoms.size() << " experience atoms";
    
    std::vector<Handle> new_concepts;
    
    if (!_enable_concept_formation) {
        logger().debug() << "[KnowledgeIntegrator] Concept formation disabled";
        return new_concepts;
    }
    
    try {
        // Simple concept formation: look for patterns in the experience atoms
        std::map<std::string, int> term_frequency;
        
        // Extract terms from experience atoms
        for (const Handle& atom : experience_atoms) {
            std::string atom_name = atom->get_name();
            
            // Simple tokenization by spaces and common delimiters
            std::istringstream iss(atom_name);
            std::string term;
            while (iss >> term) {
                // Clean up term (remove punctuation, convert to lowercase)
                std::string clean_term;
                for (char c : term) {
                    if (std::isalnum(c)) {
                        clean_term += std::tolower(c);
                    }
                }
                
                if (clean_term.length() > 2) { // Ignore very short terms
                    term_frequency[clean_term]++;
                }
            }
        }
        
        // Form concepts from frequently occurring terms
        int min_frequency = std::max(2, static_cast<int>(experience_atoms.size() * 0.3));
        
        for (const auto& pair : term_frequency) {
            if (pair.second >= min_frequency) {
                std::string concept_name = "Concept_" + pair.first;
                
                // Check if concept already exists
                if (!hasKnowledgeAbout(concept_name)) {
                    Handle new_concept = registerConcept(concept_name, 
                        "Auto-formed concept from experience patterns");
                    new_concepts.push_back(new_concept);
                    
                    logger().info() << "[KnowledgeIntegrator] Formed new concept: " << concept_name 
                                   << " (frequency: " << pair.second << ")";
                }
            }
        }
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error forming concepts: " << e.what();
    }
    
    return new_concepts;
}

std::vector<Handle> KnowledgeIntegrator::validateKnowledgeConsistency()
{
    logger().debug() << "[KnowledgeIntegrator] Validating knowledge consistency";
    
    std::vector<Handle> inconsistent_knowledge;
    
    try {
        // Check for contradictory facts
        HandleSeq all_facts;
        _atomspace->get_handles_by_type(all_facts, ATOM, true);
        
        std::map<std::string, std::vector<Handle>> fact_groups;
        
        for (const Handle& atom : all_facts) {
            std::string atom_name = atom->get_name();
            
            // Group facts by subject (simplified approach)
            size_t first_space = atom_name.find(' ');
            if (first_space != std::string::npos) {
                std::string subject = atom_name.substr(0, first_space);
                fact_groups[subject].push_back(atom);
            }
        }
        
        // Look for contradictions within fact groups
        for (const auto& group : fact_groups) {
            if (group.second.size() > 1) {
                // Check for opposing truth values or contradictory statements
                for (size_t i = 0; i < group.second.size(); ++i) {
                    for (size_t j = i + 1; j < group.second.size(); ++j) {
                        const Handle& atom1 = group.second[i];
                        const Handle& atom2 = group.second[j];
                        
                        TruthValuePtr tv1 = atom1->getTruthValue();
                        TruthValuePtr tv2 = atom2->getTruthValue();
                        
                        // Simple contradiction check: opposing truth values
                        if (tv1->get_mean() > 0.5 && tv2->get_mean() < 0.5) {
                            inconsistent_knowledge.push_back(atom1);
                            inconsistent_knowledge.push_back(atom2);
                            
                            logger().warn() << "[KnowledgeIntegrator] Inconsistency detected between: " 
                                           << atom1->get_name() << " and " << atom2->get_name();
                        }
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error validating consistency: " << e.what();
    }
    
    return inconsistent_knowledge;
}

KnowledgeIntegrator::ConfidenceLevel KnowledgeIntegrator::updateKnowledgeConfidence(
    const Handle& knowledge_atom, const std::vector<Handle>& supporting_evidence)
{
    logger().debug() << "[KnowledgeIntegrator] Updating confidence for knowledge atom: " << knowledge_atom;
    
    if (knowledge_atom == Handle::UNDEFINED) {
        logger().error() << "[KnowledgeIntegrator] Cannot update confidence for undefined atom";
        return ConfidenceLevel::VERY_LOW;
    }
    
    try {
        TruthValuePtr current_tv = knowledge_atom->getTruthValue();
        double current_strength = current_tv->get_mean();
        double current_confidence = current_tv->get_confidence();
        
        // Calculate evidence strength
        double evidence_strength = 0.0;
        int valid_evidence_count = 0;
        
        for (const Handle& evidence : supporting_evidence) {
            if (evidence != Handle::UNDEFINED) {
                TruthValuePtr evidence_tv = evidence->getTruthValue();
                evidence_strength += evidence_tv->get_mean() * evidence_tv->get_confidence();
                valid_evidence_count++;
            }
        }
        
        if (valid_evidence_count > 0) {
            evidence_strength /= valid_evidence_count;
            
            // Update confidence based on evidence
            double new_strength = (current_strength + evidence_strength) / 2.0;
            double new_confidence = std::min(1.0, current_confidence + (evidence_strength * 0.1));
            
            TruthValuePtr new_tv = SimpleTruthValue::createTV(new_strength, new_confidence);
            knowledge_atom->setTruthValue(new_tv);
            
            // Convert to ConfidenceLevel
            int confidence_percent = static_cast<int>(new_confidence * 100);
            if (confidence_percent >= 90) return ConfidenceLevel::VERY_HIGH;
            else if (confidence_percent >= 70) return ConfidenceLevel::HIGH;
            else if (confidence_percent >= 40) return ConfidenceLevel::MEDIUM;
            else if (confidence_percent >= 20) return ConfidenceLevel::LOW;
            else return ConfidenceLevel::VERY_LOW;
        }
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error updating confidence: " << e.what();
    }
    
    return ConfidenceLevel::MEDIUM;
}

int KnowledgeIntegrator::cleanupOutdatedKnowledge(int age_threshold_days)
{
    logger().debug() << "[KnowledgeIntegrator] Cleaning up knowledge older than " << age_threshold_days << " days";
    
    int cleaned_count = 0;
    
    try {
        // In a real implementation, this would:
        // 1. Check atom timestamps (if available)
        // 2. Identify low-confidence, rarely accessed knowledge
        // 3. Remove or archive outdated knowledge
        // 4. Update statistics
        
        // For now, implement a simple cleanup based on confidence
        HandleSeq all_atoms;
        _atomspace->get_handles_by_type(all_atoms, ATOM, true);
        
        for (const Handle& atom : all_atoms) {
            TruthValuePtr tv = atom->getTruthValue();
            
            // Remove very low confidence knowledge
            if (tv->get_mean() < 0.1 && tv->get_confidence() < 0.1) {
                // Mark for removal (in a full implementation)
                logger().debug() << "[KnowledgeIntegrator] Marked for cleanup: " << atom->get_name();
                cleaned_count++;
            }
        }
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error during cleanup: " << e.what();
    }
    
    return cleaned_count;
}

int KnowledgeIntegrator::importKnowledge(const std::string& source_description,
                                        const std::map<std::string, std::string>& knowledge_data)
{
    logger().info() << "[KnowledgeIntegrator] Importing knowledge from: " << source_description;
    
    int imported_count = 0;
    
    try {
        for (const auto& pair : knowledge_data) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            // Import as facts with medium confidence
            Handle fact_atom = addFact(key + ": " + value, ConfidenceLevel::MEDIUM);
            
            if (fact_atom != Handle::UNDEFINED) {
                imported_count++;
                
                // Add source metadata
                Handle source_atom = _atomspace->add_node(CONCEPT_NODE, "Source_" + source_description);
                HandleSeq source_link;
                source_link.push_back(fact_atom);
                source_link.push_back(source_atom);
                _atomspace->add_link(EVALUATION_LINK, std::move(source_link));
            }
        }
        
        logger().info() << "[KnowledgeIntegrator] Successfully imported " << imported_count << " knowledge items";
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error importing knowledge: " << e.what();
    }
    
    return imported_count;
}

std::string KnowledgeIntegrator::exportKnowledge(const std::string& export_format,
                                                KnowledgeType knowledge_filter)
{
    logger().debug() << "[KnowledgeIntegrator] Exporting knowledge in format: " << export_format;
    
    std::ostringstream exported_data;
    
    try {
        if (export_format == "json") {
            exported_data << "{\n";
            exported_data << "  \"knowledge_base\": {\n";
            exported_data << "    \"total_concepts\": " << _concept_registry.size() << ",\n";
            exported_data << "    \"active_knowledge\": " << _active_knowledge.size() << ",\n";
            exported_data << "    \"concepts\": [\n";
            
            bool first = true;
            for (const auto& pair : _concept_registry) {
                if (!first) exported_data << ",\n";
                exported_data << "      {\n";
                exported_data << "        \"name\": \"" << pair.first << "\",\n";
                exported_data << "        \"handle\": \"" << pair.second << "\"\n";
                exported_data << "      }";
                first = false;
            }
            
            exported_data << "\n    ]\n";
            exported_data << "  }\n";
            exported_data << "}";
            
        } else if (export_format == "text") {
            exported_data << "Knowledge Base Export\n";
            exported_data << "====================\n\n";
            exported_data << "Total Concepts: " << _concept_registry.size() << "\n";
            exported_data << "Active Knowledge Items: " << _active_knowledge.size() << "\n\n";
            exported_data << "Concepts:\n";
            
            for (const auto& pair : _concept_registry) {
                exported_data << "- " << pair.first << " [" << pair.second << "]\n";
            }
            
        } else {
            logger().error() << "[KnowledgeIntegrator] Unsupported export format: " << export_format;
            return "";
        }
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error exporting knowledge: " << e.what();
        return "";
    }
    
    return exported_data.str();
}

std::vector<Handle> KnowledgeIntegrator::getMostActiveKnowledge(int count)
{
    logger().debug() << "[KnowledgeIntegrator] Getting " << count << " most active knowledge items";
    
    std::vector<Handle> most_active;
    
    try {
        // For now, return most recently accessed items from active knowledge set
        most_active.reserve(std::min(count, static_cast<int>(_active_knowledge.size())));
        
        auto it = _active_knowledge.begin();
        for (int i = 0; i < count && it != _active_knowledge.end(); ++i, ++it) {
            most_active.push_back(*it);
        }
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error getting active knowledge: " << e.what();
    }
    
    return most_active;
}

TruthValuePtr KnowledgeIntegrator::assessKnowledgeReliability(const Handle& knowledge_atom)
{
    if (knowledge_atom == Handle::UNDEFINED) {
        return SimpleTruthValue::createTV(0.0, 0.0);
    }
    
    try {
        TruthValuePtr current_tv = knowledge_atom->getTruthValue();
        
        // Simple reliability assessment based on:
        // 1. Current truth value strength
        // 2. Number of supporting links
        // 3. Age of the knowledge (simplified)
        
        double strength = current_tv->get_mean();
        double confidence = current_tv->get_confidence();
        
        // Factor in connectivity (more connections = more reliable)
        const IncomingSet& incoming = knowledge_atom->getIncomingSet();
        double connectivity_factor = std::min(1.0, incoming.size() * 0.1);
        
        double reliability_strength = (strength + connectivity_factor) / 2.0;
        double reliability_confidence = std::min(1.0, confidence + (connectivity_factor * 0.2));
        
        return SimpleTruthValue::createTV(reliability_strength, reliability_confidence);
        
    } catch (const std::exception& e) {
        logger().error() << "[KnowledgeIntegrator] Error assessing reliability: " << e.what();
        return SimpleTruthValue::createTV(0.0, 0.0);
    }
}