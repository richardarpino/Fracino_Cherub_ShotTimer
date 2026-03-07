#ifndef ITAG_PROCESSOR_H
#define ITAG_PROCESSOR_H

/**
 * Interface for components that calculate derived states from Registry tags.
 */
class ITagProcessor {
public:
    virtual ~ITagProcessor() = default;

    /**
     * Executes the calculation/logic. 
     * Usually reads one tag and publishes another.
     */
    virtual void update() = 0;
};

#endif
