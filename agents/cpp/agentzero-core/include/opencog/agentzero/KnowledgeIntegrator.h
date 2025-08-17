/*
 * opencog/agentzero/KnowledgeIntegrator.h
 *
 * Copyright (C) 2024 OpenCog Foundation
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Knowledge Integrator Implementation
 * Bridges knowledge representation with AtomSpace operations
 * Part of the AGENT-ZERO-GENESIS project
 */

#ifndef _OPENCOG_AGENTZERO_KNOWLEDGE_INTEGRATOR_H
#define _OPENCOG_AGENTZERO_KNOWLEDGE_INTEGRATOR_H

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/truthvalue/TruthValue.h>
#include <opencog/util/Logger.h>

namespace opencog {
namespace agentzero {

class AgentZeroCore;

/**
 * KnowledgeIntegrator - Bridges knowledge representation with AtomSpace
 *
 * This class provides comprehensive knowledge management capabilities,
 * integrating various forms of knowledge representation with OpenCog's
 * AtomSpace. It handles knowledge acquisition, storage, retrieval,
 * and reasoning integration.
 *
 * Key features:
 * - Knowledge representation in AtomSpace
 * - Concept formation and relationship mapping
 * - Semantic knowledge integration
 * - Memory consolidation and organization
 * - Query and retrieval interfaces
 */
class KnowledgeIntegrator
{
public:
    // Knowledge types for categorization
    enum class KnowledgeType {
        FACTUAL,        // Facts about the world
        PROCEDURAL,     // How to perform actions
        EPISODIC,       // Experience memories
        SEMANTIC,       // Concept relationships
        CONDITIONAL,    // If-then rules
        TEMPORAL        // Time-based knowledge
    };
    
    // Confidence levels for knowledge assertions
    enum class ConfidenceLevel {
        VERY_LOW = 0,
        LOW = 25,
        MEDIUM = 50,
        HIGH = 75,
        VERY_HIGH = 100
    };

private:
    // Core references
    AgentZeroCore* _agent_core;
    AtomSpacePtr _atomspace;
    
    // Knowledge organization structures
    std::map<KnowledgeType, Handle> _knowledge_categories;
    std::map<std::string, Handle> _concept_registry;
    std::set<Handle> _active_knowledge;
    
    // AtomSpace handles for knowledge contexts
    Handle _knowledge_base;
    Handle _working_knowledge;
    Handle _semantic_network;
    Handle _episodic_memory;
    Handle _procedural_memory;
    
    // Configuration
    bool _enable_concept_formation;
    bool _enable_semantic_integration;
    bool _enable_memory_consolidation;
    double _knowledge_threshold;
    
    // Internal methods
    void initializeKnowledgeStructures();
    Handle createKnowledgeAtom(const std::string& content, KnowledgeType type);
    Handle findOrCreateConcept(const std::string& concept_name);
    void establishConceptRelation(const Handle& concept1, const Handle& concept2, 
                                 const std::string& relation_type);
    TruthValuePtr assessKnowledgeReliability(const Handle& knowledge_atom);
    void consolidateMemory();
    std::vector<Handle> findRelatedKnowledge(const Handle& query_atom);

public:
    /**
     * Constructor
     * @param agent_core Pointer to the parent AgentZeroCore instance
     * @param atomspace Shared pointer to the AtomSpace
     */
    KnowledgeIntegrator(AgentZeroCore* agent_core, AtomSpacePtr atomspace);
    
    /**
     * Destructor - cleans up knowledge management resources
     */
    ~KnowledgeIntegrator();
    
    // Knowledge acquisition
    /**
     * Add new factual knowledge to the knowledge base
     * @param fact_description Description of the fact
     * @param confidence Confidence level in the fact
     * @return Handle to the created knowledge atom
     */
    Handle addFact(const std::string& fact_description, 
                   ConfidenceLevel confidence = ConfidenceLevel::MEDIUM);
    
    /**
     * Add procedural knowledge (how to do something)
     * @param procedure_description Description of the procedure
     * @param steps Vector of step descriptions
     * @param confidence Confidence in the procedure
     * @return Handle to the created procedural knowledge
     */
    Handle addProcedure(const std::string& procedure_description,
                       const std::vector<std::string>& steps,
                       ConfidenceLevel confidence = ConfidenceLevel::MEDIUM);
    
    /**
     * Add episodic memory (experience-based knowledge)
     * @param experience_description Description of the experience
     * @param context_atoms Related context atoms
     * @param confidence Confidence in the memory
     * @return Handle to the created episodic memory
     */
    Handle addEpisode(const std::string& experience_description,
                     const std::vector<Handle>& context_atoms,
                     ConfidenceLevel confidence = ConfidenceLevel::MEDIUM);
    
    /**
     * Add semantic relationship between concepts
     * @param concept1_name Name of the first concept
     * @param relation_type Type of relationship (e.g., "isa", "has", "causes")
     * @param concept2_name Name of the second concept
     * @param confidence Confidence in the relationship
     * @return Handle to the created relationship
     */
    Handle addSemanticRelation(const std::string& concept1_name,
                              const std::string& relation_type,
                              const std::string& concept2_name,
                              ConfidenceLevel confidence = ConfidenceLevel::MEDIUM);
    
    // Knowledge retrieval
    /**
     * Query knowledge base with natural language query
     * @param query_text Natural language query
     * @param max_results Maximum number of results to return
     * @return Vector of relevant knowledge atoms
     */
    std::vector<Handle> queryKnowledge(const std::string& query_text, 
                                      int max_results = 10);
    
    /**
     * Find facts related to a specific concept
     * @param concept_name Name of the concept
     * @return Vector of related fact atoms
     */
    std::vector<Handle> getFactsAbout(const std::string& concept_name);
    
    /**
     * Get procedural knowledge for a specific task
     * @param task_description Description of the task
     * @return Vector of relevant procedure atoms
     */
    std::vector<Handle> getProceduresFor(const std::string& task_description);
    
    /**
     * Retrieve episodic memories related to a context
     * @param context_atoms Context atoms to match against
     * @return Vector of relevant episodic memory atoms
     */
    std::vector<Handle> getEpisodesRelatedTo(const std::vector<Handle>& context_atoms);
    
    /**
     * Get semantic relationships for a concept
     * @param concept_name Name of the concept
     * @param relation_type Optional specific relation type filter
     * @return Vector of relationship atoms
     */
    std::vector<Handle> getSemanticRelations(const std::string& concept_name,
                                            const std::string& relation_type = "");
    
    // Concept management
    /**
     * Register a new concept in the knowledge base
     * @param concept_name Name of the concept
     * @param concept_description Optional description
     * @return Handle to the concept atom
     */
    Handle registerConcept(const std::string& concept_name, 
                          const std::string& concept_description = "");
    
    /**
     * Check if a concept exists in the knowledge base
     * @param concept_name Name of the concept
     * @return true if concept exists
     */
    bool hasKnowledgeAbout(const std::string& concept_name);
    
    /**
     * Get all known concepts
     * @return Vector of concept atom handles
     */
    std::vector<Handle> getAllConcepts();
    
    /**
     * Form new concepts from experience patterns
     * @param experience_atoms Vector of experience atoms to analyze
     * @return Vector of newly formed concept atoms
     */
    std::vector<Handle> formConceptsFrom(const std::vector<Handle>& experience_atoms);
    
    // Knowledge validation and maintenance
    /**
     * Validate knowledge consistency
     * @return Vector of inconsistent knowledge atoms found
     */
    std::vector<Handle> validateKnowledgeConsistency();
    
    /**
     * Update knowledge confidence based on new evidence
     * @param knowledge_atom Handle to knowledge to update
     * @param supporting_evidence Vector of supporting evidence atoms
     * @return Updated confidence level
     */
    ConfidenceLevel updateKnowledgeConfidence(const Handle& knowledge_atom,
                                             const std::vector<Handle>& supporting_evidence);
    
    /**
     * Remove or deprecate outdated knowledge
     * @param age_threshold_days Knowledge older than this will be reviewed
     * @return Number of knowledge items processed
     */
    int cleanupOutdatedKnowledge(int age_threshold_days = 30);
    
    // Integration interfaces
    /**
     * Import knowledge from external sources
     * @param source_description Description of the knowledge source
     * @param knowledge_data Structured knowledge data
     * @return Number of knowledge items imported
     */
    int importKnowledge(const std::string& source_description,
                       const std::map<std::string, std::string>& knowledge_data);
    
    /**
     * Export knowledge to external format
     * @param export_format Format for export (e.g., "json", "rdf", "text")
     * @param knowledge_filter Optional filter for specific knowledge types
     * @return Serialized knowledge data
     */
    std::string exportKnowledge(const std::string& export_format,
                               KnowledgeType knowledge_filter = KnowledgeType::FACTUAL);
    
    // Statistics and monitoring
    /**
     * Get knowledge base statistics
     * @return Map of statistics (concept_count, fact_count, etc.)
     */
    std::map<std::string, int> getKnowledgeStatistics();
    
    /**
     * Get the most active knowledge concepts
     * @param count Number of top concepts to return
     * @return Vector of most frequently accessed concept atoms
     */
    std::vector<Handle> getMostActiveKnowledge(int count = 10);
    
    // Configuration
    /**
     * Set knowledge acceptance threshold
     * @param threshold Minimum confidence for accepting new knowledge (0.0-1.0)
     */
    void setKnowledgeThreshold(double threshold) { _knowledge_threshold = threshold; }
    
    /**
     * Enable or disable automatic concept formation
     * @param enable Whether to automatically form concepts from patterns
     */
    void setConceptFormationEnabled(bool enable) { _enable_concept_formation = enable; }
    
    // AtomSpace integration
    /**
     * Get the knowledge base root atom
     * @return Handle to knowledge base root
     */
    Handle getKnowledgeBase() const { return _knowledge_base; }
    
    /**
     * Get the semantic network atom
     * @return Handle to semantic network
     */
    Handle getSemanticNetwork() const { return _semantic_network; }
    
    /**
     * Get status information for debugging
     * @return JSON string with status details
     */
    std::string getStatusInfo() const;
    
    /**
     * Process knowledge integration for one cycle
     * Called by the cognitive loop
     * @return true if processing completed successfully
     */
    bool processKnowledgeIntegration();
};

} // namespace agentzero
} // namespace opencog

#endif // _OPENCOG_AGENTZERO_KNOWLEDGE_INTEGRATOR_H