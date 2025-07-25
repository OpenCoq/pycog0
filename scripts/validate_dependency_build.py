#!/usr/bin/env python3
"""
Test script for OpenCog Dependency Build Matrix workflow validation
"""

import os
import sys
import yaml
import subprocess
from pathlib import Path

def validate_workflow_syntax():
    """Validate YAML syntax of the workflow file"""
    workflow_path = Path(".github/workflows/opencog-dependency-build.yml")
    
    if not workflow_path.exists():
        print("❌ Workflow file not found")
        return False
        
    try:
        with open(workflow_path) as f:
            workflow = yaml.safe_load(f)
        print("✅ Workflow YAML syntax is valid")
        return True
    except yaml.YAMLError as e:
        print(f"❌ YAML syntax error: {e}")
        return False

def validate_component_directories():
    """Check if component directories exist"""
    components = {
        'foundation': ['cogutil', 'moses', 'blender_api_msgs', 'external-tools', 'ocpkg'],
        'core': ['atomspace', 'atomspace-rocks', 'atomspace-ipfs', 'atomspace-websockets', 
                'atomspace-restful', 'atomspace-bridge', 'atomspace-metta', 'atomspace-rpc', 
                'atomspace-cog', 'atomspace-agents', 'atomspace-dht'],
        'logic': ['ure'],  # unify is external
        'cognitive': ['cogserver', 'attention', 'spacetime', 'pattern-index', 
                     'dimensional-embedding', 'profile'],
        'advanced': ['pln', 'miner', 'asmoses', 'benchmark'],
        'learning': ['learn', 'generate', 'language-learning'],
        'language': ['lg-atomese', 'relex', 'link-grammar'],
        'robotics': ['vision', 'perception', 'sensory', 'ros-behavior-scripting', 
                    'robots_config', 'pau2motors'],
        'integration': ['opencog']
    }
    
    missing_components = []
    existing_components = []
    
    for category, component_list in components.items():
        for component in component_list:
            if Path(component).exists():
                existing_components.append(f"{category}: {component}")
            else:
                missing_components.append(f"{category}: {component}")
    
    print(f"✅ Found {len(existing_components)} existing components")
    if missing_components:
        print(f"⚠️  Missing {len(missing_components)} components:")
        for component in missing_components[:10]:  # Show first 10
            print(f"   - {component}")
        if len(missing_components) > 10:
            print(f"   ... and {len(missing_components) - 10} more")
    
    return len(existing_components) > 0

def validate_dependency_structure():
    """Validate that the dependency structure makes sense"""
    workflow_path = Path(".github/workflows/opencog-dependency-build.yml")
    
    with open(workflow_path) as f:
        workflow = yaml.safe_load(f)
    
    jobs = workflow.get('jobs', {})
    
    # Check job dependencies
    expected_deps = {
        'foundation': [],
        'core': ['foundation'],
        'logic': ['foundation', 'core'],
        'cognitive': ['foundation', 'core', 'logic'],
        'advanced': ['foundation', 'core', 'logic', 'cognitive'],
        'learning': ['foundation', 'core', 'logic', 'cognitive'],
        'language': ['foundation', 'core'],
        'robotics': ['foundation', 'core'],
        'integration': ['foundation', 'core', 'logic', 'cognitive', 'advanced', 'learning']
    }
    
    dependency_issues = []
    
    for job_name, expected_needs in expected_deps.items():
        if job_name in jobs:
            actual_needs = jobs[job_name].get('needs', [])
            if isinstance(actual_needs, str):
                actual_needs = [actual_needs]
            
            # Check if all expected dependencies are present
            missing_deps = set(expected_needs) - set(actual_needs)
            if missing_deps:
                dependency_issues.append(f"{job_name}: missing dependencies {missing_deps}")
    
    if dependency_issues:
        print("⚠️  Dependency structure issues:")
        for issue in dependency_issues:
            print(f"   - {issue}")
    else:
        print("✅ Dependency structure is correct")
    
    return len(dependency_issues) == 0

def main():
    """Main validation function"""
    print("🔍 Validating OpenCog Dependency Build Matrix...")
    print("=" * 50)
    
    checks = [
        ("Workflow YAML syntax", validate_workflow_syntax),
        ("Component directories", validate_component_directories),
        ("Dependency structure", validate_dependency_structure)
    ]
    
    results = []
    for check_name, check_func in checks:
        print(f"\n📋 {check_name}:")
        try:
            result = check_func()
            results.append(result)
        except Exception as e:
            print(f"❌ Error during {check_name}: {e}")
            results.append(False)
    
    print("\n" + "=" * 50)
    print("📊 Validation Summary:")
    
    passed = sum(results)
    total = len(results)
    
    for i, (check_name, _) in enumerate(checks):
        status = "✅ PASS" if results[i] else "❌ FAIL"
        print(f"   {check_name}: {status}")
    
    print(f"\n🎯 Overall: {passed}/{total} checks passed")
    
    if passed == total:
        print("🎉 All validations passed! The workflow is ready to use.")
        return 0
    else:
        print("⚠️  Some validations failed. Please review the issues above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())